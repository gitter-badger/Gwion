#include <string.h>
#include "defs.h"
#include "map.h"
#include "func.h"

#define GWION_DOC_DIR "/usr/lib/Gwion/doc/"
typedef struct {
  Env env;
  Context ctx;
  FILE* html;
  FILE* data;
} Doc;

typedef struct {
  Env env;
  Context ctx;
  FILE *api, *tag, *tok;
} Textadept;

#define GWION_API_DIR "/usr/lib/Gwion/api/"
#define GWION_TAG_DIR "/usr/lib/Gwion/tags/"
#define GWION_TOK_DIR "/usr/lib/Gwion/tok/"

static m_str usable(m_str name)
{
  m_uint i;
  m_str str = strdup(name);
  for(i = 0; i < strlen(str); i++) {
    if(str[i] == '/' || str[i] == '.' || str[i] == '@')
      str[i] = '_';
    else if(str[i] >= 'A' && str[i] <= 'Z')
      str[i] -= 'A' - 'a';
  }
  return str;
}

/**
* @brief find context from name
*
* @param env Gwion Environmen
* @param name name of the context
*
* @return found Context or NULL if not found
*/
static Context find_context(Env env, m_str name)
{
  Context ctx;
  if((ctx = (Context)map_get(env->known_ctx, (vtype)insert_symbol(name))))
    return ctx;
  return NULL;
}

static Textadept* new_Textadept(Env env, m_str str)
{
// get rid of extension
  char c[1024];
  m_str name;
  Textadept* doc = malloc(sizeof(Textadept));
  doc->env = env;
  doc->ctx = find_context(env, str);
  if(!doc->ctx) {
    free(doc);
    return NULL;
  }
  name = doc->ctx != env->global_context ?
         usable(doc->ctx->filename) : strdup(env->global_nspc->name);
  memset(c, 0, 1024);
  strcat(c, GWION_API_DIR);
  strcat(c, name);
  strcat(c, ".api");
  doc->api = fopen(c, "w");
  memset(c, 0, 1024);
  strcat(c, GWION_TAG_DIR);
  strcat(c, name);
  strcat(c, ".tag");
  doc->tag = fopen(c, "w");
  memset(c, 0, 1024);
  strcat(c, GWION_TOK_DIR);
  strcat(c, name);
  strcat(c, ".tok");
  doc->tok = fopen(c, "w");
  free(name);
  return doc;
}

static void free_Textadept(Textadept* a)
{
  fclose(a->api);
  fclose(a->tag);
  fclose(a->tok);
}

static Doc* new_Doc(Env env, m_str str)
{
  char c[1024];
  m_str name;
  Doc* doc = malloc(sizeof(Doc));
  doc->env = env;
  doc->ctx = find_context(env, str);
  if(!doc->ctx) {
    free(doc);
    return NULL;
  }
  name = doc->ctx != env->global_context ?
         usable(doc->ctx->filename) : strdup(env->global_nspc->name);
  memset(c, 0, 1024);
  strcat(c, GWION_DOC_DIR);
  strcat(c, name);
  strcat(c, ".html");
  doc->html = fopen(c, "w");
  memset(c, 0, 1024);
  strcat(c, GWION_DOC_DIR);
  strcat(c, "dat/");
  strcat(c, name);
  strcat(c, ".js");
  doc->data = fopen(c, "w");
  free(name);
  return doc;
}

static void free_Doc(Doc* a)
{
  fclose(a->data);
  fclose(a->html);
  free(a);
}

static m_str getfull(Doc* doc, NameSpace nspc, m_str name)
{
  m_uint i;
  char tmp[2054];
  m_str ret;
  Vector v = new_Vector();
  if(name)
    vector_append(v, (vtype)name);
  memset(tmp, 0, 2054);
  while(nspc) {
    if(nspc == doc->ctx->nspc || nspc == doc->env->global_nspc)
      break;
    vector_append(v, (vtype)nspc->name);
    nspc = nspc->parent;
  }
  strcat(tmp, (m_str)vector_back(v));
  for(i = vector_size (v) - 1; i > 0; i--) {
    strcat(tmp, "_");
    strcat(tmp, (m_str)vector_at(v, i - 1));
  }
  ret = usable(tmp);
  free(v);
  return ret;
}

static m_str print_type(Type t)
{
  int i;
  char str[2054];
  m_str ret;
  memset(str, 0, 2054);
  strcat(str, t->name);
  for(i = 0; i < t->array_depth; i++)
    strcat(str, "[]");
  ret = strdup(str); // coub be better
  return ret;
}
// t(ype) f(function) v(ariable)
static void mkadt_value(Textadept* a, Value v)
{
  fprintf(a->api, "%s %s\n", v->name, v->doc);
  if(v->owner != a->env->global_nspc)
    fprintf(a->tag, "%s _ 0;\"\tm class:%s\n", v->name, v->owner->name);
  else
    fprintf(a->tag, "%s _ 0;\"\tm\n", v->name);
  fprintf(a->tok, "v %s\n", v->name);
}

static void mkadt_func(Textadept* a, Func f)
{
  m_str c = strdup(f->name);
  c = strsep(&c, "@");
  fprintf(a->api, "%s %s\n", c, f->doc);
  /*  if(f->value_ref->owner != a->ctx->nspc)*/
  /*    fprintf(a->tag, "%s _ 0;\"\tf class:%s\n", c, f->value_ref->owner->name);*/
  /*  else*/
  fprintf(a->tag, "%s _ 0;\"\tf\n", c);
  fprintf(a->tok, "f %s\n", c);
  free(c);
}

static void mkadt_nspc(Textadept* doc, NameSpace nspc);

static void mkadt_type(Textadept* a, Type t)
{
  fprintf(a->api, "%s %s\n", t->name, t->doc);
  fprintf(a->tag, "%s _ 0;\"\tt\n", t->name);
  if(isprim(t) > 0)
    fprintf(a->tok, "p %s\n", t->name);
  else if(isa(t, &t_ugen) > 0)
    fprintf(a->tok, "u %s\n", t->name);
  else if(isa(t, &t_event) > 0)
    fprintf(a->tok, "e %s\n", t->name);
  else
    fprintf(a->tok, "o %s\n", t->name);
  if(t->info)
    mkadt_nspc(a, t->info);
}

static void mkdoc_value(Doc* doc, Value v)
{
  m_str full = getfull(doc, v->owner, v->name);
  m_str type  = v->m_type->array_type ?
                getfull(doc, v->m_type->array_type->owner, v->m_type->name) :
                getfull(doc, v->m_type->owner, v->m_type->name);
  m_str file = v->m_type->array_type ?
               usable(v->m_type->array_type->owner->filename) :
               v->m_type->owner ?
               usable(v->m_type->owner->filename) :
               usable("global_nspc");
  m_str p_type = print_type(v->m_type);
//m_str p_type = print_type(v->m_type);
  fprintf(doc->html, "<a class=\"anchor\" id=\"%s\"> </a><p class=\"first\"><li><a class=\"reference external\" href=\"%s.html#%s\">%s</a> <strong>%s</strong>",
          //  full, file, type, p_type, v->name);
          full, file, type, p_type, v->name);
//free(p_type);
  if(v->doc)
    fprintf(doc->html, "<em> %s</em></li></p>\n", v->doc);
  else
    fprintf(doc->html, "</li></p>\n");
//  fprintf(doc->data, "['%s', ['%s', ['../%s.html#%s', 1, ' \[variable\%s in <b>%s</b> <em>%s</em> '] ]],\n",
//    v->name, v->name, file, full, "]", v->owner->name, v->doc ? v->doc : "");
  char * str = v->doc ? strndup(v->doc, 16) : "";
  fprintf(doc->data, "['%s', ['%s', ['../%s.html#%s', 1, ' \[variable\%s in <b>%s</b> <em>%s</em> '] ]],\n",
          v->name, v->name, file, full, "]", v->owner->name, str);
  if(v->doc)
    free(str);
  free(full);
  free(type);
  free(file);
  free(p_type);
}

static void mkdoc_func(Doc* doc, Func f)
{
  // hack while func ptr not fixed
  if(f->value_ref->is_const)
    return;
  m_str name = strdup(S_name(f->def->name));
  m_str ufile = usable(doc->ctx->filename);
  m_str full = getfull(doc, f->value_ref->owner, f->name);
  m_str file = f->def->ret_type->array_type ?
               usable(f->def->ret_type->array_type->owner->filename) :
               usable(f->def->ret_type->owner->filename);
  m_str type  = f->def->ret_type->array_type ?
                getfull(doc, f->def->ret_type->array_type->owner, f->def->ret_type->name) :
                getfull(doc, f->def->ret_type->owner, f->def->ret_type->name);
  m_str p_type = print_type(f->def->ret_type);

  name = strsep(&name, "@");
  fprintf(doc->html, "<a class=\"anchor\" id=\"%s\"> </a><p class=\"first\"><a class=\"reference external\" href=\"%s.html#%s\"><li>%s</a> <strong>%s</strong>",
          full, file, type, p_type, name);
  if(f->doc)
    fprintf(doc->html, "<em> %s</em>\n", f->doc);
  else
    fprintf(doc->html, "\n");

  fprintf(doc->html, "<blockquote><ol class=\"arabic simple\">\n\n");
  Arg_List arg = f->def->arg_list;
  while(arg) {
    Value v = arg->var_decl->value;
    char a_full[1024];
    m_str a_type  = v->m_type->array_type ?
                    getfull(doc, v->m_type->array_type->owner, v->m_type->name) :
                    getfull(doc, v->m_type->owner, v->m_type->name);
    m_str a_file = v->m_type->array_type ?
                   usable(v->m_type->array_type->owner->filename) :
                   usable(v->m_type->owner->filename);
    m_str ap_type = print_type(v->m_type);
    memset(a_full, 0, 1024);
    strcat(a_full, full);
    strcat(a_full, "_");
    strcat(a_full, v->name);
    fprintf(doc->html, "<a class=\"anchor\" id=\"%s\"> </a><p class=\"first\"><li><a class=\"reference external\" href=\"%s.html#%s\">%s</a> <strong>%s</strong>",
            a_full, a_file, a_type, ap_type, v->name);
//    if(v->doc)
//      fprintf(doc->html, "<em>%s</em></li></p>\n", v->doc);
    if(arg->doc)
      fprintf(doc->html, "<em>%s</em></li></p>\n", arg->doc);
    else
      fprintf(doc->html, "</li></p>\n");
    free(a_type);
    free(a_file);
    free(ap_type);
    m_str str = v->doc ? strndup(v->doc, 16) : "";
    fprintf(doc->data, "['%s', ['%s', ['../%s.html#%s', 1, ' \[argument\%s in <b>%s</b> <em>%s</em> '] ]],\n",
            v->name, v->name, file, a_full, "]", v->owner->name, str);
    if(v->doc)
      free(str);
    arg = arg->next;
  }
  fprintf(doc->html, "</ol></blockquote></li></p>\n\n");
  m_str str = f->doc ? strndup(f->doc, 16) : "";
//  m_str str2 = strdup(full);
//  str2 = strsep(&str2, "@");
  fprintf(doc->data, "['%s', ['%s', ['../%s.html#%s', 1, ' \[function\%s in <b>%s</b> <em>%s</em> '] ]],\n",
          name, name, file, full, "]", f->value_ref->owner->name, str);

  if(f->doc)
    free(str);
//  free(str2);
  free(ufile);
  free(name);
  free(full);
  free(type);
  free(file);
  free(p_type);
}

static void mkdoc_type(Doc* doc, Type t);

static void mkadt_nspc(Textadept* doc, NameSpace nspc)
{
  int i;
  Vector type    = new_Vector();
  Vector m_value = new_Vector();
  Vector s_value = new_Vector();
  Vector m_func  = new_Vector();
  Vector s_func  = new_Vector();
  Vector v = scope_get(nspc->value);
  for(i = 0; i < vector_size(v); i++) {
    Value value = (Value)vector_at(v, i);

    if(value->func_ref)
      continue;
    else if(isa(value->m_type, &t_class) < 0) {
      if(!value->is_member)
        vector_append(s_value, (vtype)value);
      else
        vector_append(m_value, (vtype)value);
    } else
      vector_append(type, (vtype)value);
  }
  free(v);
  v = scope_get(nspc->func);
  for(i = 0; i < vector_size(v); i++) {
    Func func = (Func)vector_at(v, i);
    if(!func->is_member)
      vector_append(s_func, (vtype)func);
    else
      vector_append(m_func, (vtype)func);
  }
  free(v);
  for(i = 0; i < vector_size(type); i++)
    mkadt_type(doc, ((Value)vector_at(type, i))->m_type->actual_type);
  free(type);

  for(i = 0; i < vector_size(m_value); i++)
    mkadt_value(doc, (Value)vector_at(m_value, i));
  free_Vector(m_value);

  for(i = 0; i < vector_size(s_value); i++)
    mkadt_value(doc, (Value)vector_at(s_value, i));
  free_Vector(s_value);

  for(i = 0; i < vector_size(m_func); i++)
    mkadt_func(doc, (Func)vector_at(m_func, i));
  free_Vector(m_func);

  for(i = 0; i < vector_size(s_func); i++)
    mkadt_func(doc, (Func)vector_at(s_func, i));
  free_Vector(s_func);
}
static void mkdoc_nspc(Doc* doc, NameSpace nspc)
{
  printf("mkdoc nspc '%s'\n", nspc->name);
  int i;
  Vector type    = new_Vector();
  Vector m_value = new_Vector();
  Vector s_value = new_Vector();
  Vector m_func  = new_Vector();
  Vector s_func  = new_Vector();

  Vector v = (Vector)scope_get(nspc->value);
  for(i = 0; i < vector_size(v); i++) {

    Value value = (Value)vector_at(v, i);

    if(value->func_ref)
      continue;
    if(!value->m_type) {
      printf("value->name %s\n", value->name);
      continue;
    }
    printf("value->name %s\n", value->name);
    printf("value->type->name %s\n", value->m_type->name);
//       else
    if(isa(value->m_type, &t_class) < 0) {
      if(!value->is_member)
        vector_append(s_value, (vtype)value);
      else
        vector_append(m_value, (vtype)value);
    } else
      vector_append(type, (vtype)value);
  }
  free(v);

  /*
      v = scope_get(nspc->func);
      for(i = 0; i < vector_size(v); i++) {
          Func func = (Func)vector_at(v, i);
          if(!func->is_member)
              vector_append(s_func, (vtype)func);
          else
              vector_append(m_func, (vtype)func);
      }
      free(v);
  */
  fprintf(doc->html, "<div class=\"section\"><h3>Types</h3><blockquote>\n");
  for(i = 0; i < vector_size(type); i++)
    mkdoc_type(doc, ((Value)vector_at(type, i))->m_type->actual_type);
  free(type);
  fprintf(doc->html, "</blockquote></div>\n");

  fprintf(doc->html, "<div class=\"section\"><h3>Member Variables</h3><blockquote>\n");
  for(i = 0; i < vector_size(m_value); i++)
    mkdoc_value(doc, (Value)vector_at(m_value, i));
  free_Vector(m_value);
  fprintf(doc->html, "</blockquote></div>\n");

  fprintf(doc->html, "<div class=\"section\"><h3>Static Variables</h3><blockquote>\n");
  for(i = 0; i < vector_size(s_value); i++)
    mkdoc_value(doc, (Value)vector_at(s_value, i));
  free_Vector(s_value);
  fprintf(doc->html, "</blockquote></div>\n");

  fprintf(doc->html, "<div class=\"section\"><h3>Member Func</h3><blockquote>\n");
  for(i = 0; i < vector_size(m_func); i++)
    mkdoc_func(doc, (Func)vector_at(m_func, i));
  free_Vector(m_func);
  fprintf(doc->html, "</blockquote></div>\n");

  fprintf(doc->html, "<div class=\"section\"><h3>Static Func</h3><blockquote>\n");
  for(i = 0; i < vector_size(s_func); i++)
    mkdoc_func(doc, (Func)vector_at(s_func, i));
  free_Vector(s_func);
  fprintf(doc->html, "</blockquote></div>\n");
}

static m_str t_last = "";
static void mkdoc_type(Doc* doc, Type t)
{
  m_str name;
  m_str full;
  m_str file;
  m_str p_full;
  m_str p_file;

  // array depth is good, but t_last is weird
  if(t->array_depth || !strcmp(t_last, t->name))
    return;
  t_last = t->name;
  name   = usable(t->name);
  full   = getfull(doc, t->owner, t->name);
  file   = usable(t->owner->filename);
  p_full = t->parent ? getfull(doc, t->parent->owner, t->parent->name) : strdup("");
  p_file = t->parent ? usable(t->parent->owner->filename) : strdup("");

  fprintf(doc->html, "<div class=\"section\"><h2><a class=\"anchor\" id=\"%s\">%s</a> <em>%s</em> <a href=\"%s.html#%s\">%s</a></h2><em>%s</em><blockquote>\n",
          full, t->name, t->parent ? "extends" : "", p_file, p_full,
          t->parent ? t->parent->name : "", t->doc ? t->doc : "");

  if(t->info)
    mkdoc_nspc(doc, t->info);
  fprintf(doc->html, "</blockquote></div>\n");
  m_str str = t->doc ? strndup(t->doc, 16) : "";
  fprintf(doc->data, "['%s', ['%s', ['../%s.html#%s', 1, ' \[type\%s in <b>%s</b> <em>%s</em> '] ]],\n",
          name, t->name, file, full, "]", t->owner->name, str);

  if(t->doc)
    free(str);
  free(name);
  free(full);
  free(file);
  free(p_full);
  free(p_file);
}

void mkadt_context(Env env, m_str str)
{
  int i;
  Textadept* doc = new_Textadept(env, str);
  for(i = 0; i < vector_size(doc->ctx->new_types); i++)
    mkadt_type(doc, (Type)vector_at(doc->ctx->new_types, i));
  for(i = 0; i < vector_size(doc->ctx->new_values); i++)
    mkadt_value(doc, ((Value)vector_at(doc->ctx->new_values, i)));
  for(i = 0; i < vector_size(doc->ctx->new_funcs); i++)
    mkadt_func(doc, ((Func)vector_at(doc->ctx->new_funcs, i)));
  free_Textadept(doc);
}

void mkdoc_context(Env env, m_str str)
{
  int i;
  Doc* doc = new_Doc(env, str);
  if(!doc)
    return;
  fprintf(doc->html, "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>\
<title>Gwion: %s</title>\
<html><body>\
<section class=\"page-header\">\
<h1 class=\"project-name\">Gwion</h1>\
<h2 class=\"project-tagline\">A strongly timed musical programming language</h2>\
</section>\
<script src=\"dynsections.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\
<script src=\"jquery.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\
<script src=\"searchdata.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\
<link rel=\"stylesheet\" href=\"gwion.css\">\
<link rel=\"stylesheet\" href=\"normalize.css\">\
<link href=\"search/search.css\" rel=\"stylesheet\" type=\"text/css\"/>\
<script src=\"search/list.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\
<script src=\"search/search.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\
<script type=\"text/javascript\">$(document).ready(function() { searchBox.OnSelectItem(0); });</script>\
<script type=\"text/javascript\">var searchBox = new SearchBox(\"searchBox\", \"search\",false,'Search');</script>\
<div id=\"MSearchBox\" class=\"MSearchBoxInactive\">\
  <span class=\"left\">\
    <img id=\"MSearchSelect\" src=\"search/mag_sel.png\"\
      onmouseover=\"return searchBox.OnSearchSelectShow()\"\
      onmouseout=\"return searchBox.OnSearchSelectHide()\"\
      alt=\"\"/>\
    <input type=\"text\" id=\"MSearchField\" value=\"Search\" accesskey=\"S\"\
      onfocus=\"searchBox.OnSearchFieldFocus(true)\"\
      onblur=\"searchBox.OnSearchFieldFocus(false)\"\
      onkeyup=\"searchBox.OnSearchFieldChange(event)\"/>\
  </span>\
  <span class=\"right\">\
    <a id=\"MSearchClose\" href=\"javascript:searchBox.CloseResultsWindow()\"><img id=\"MSearchCloseImg\" border=\"0\" sr$\
  </span>\
</div>\
<div id=\"doc-content\">\
<!-- window showing the filter options -->\
<div id=\"MSearchSelectWindow\"\
     onmouseover=\"return searchBox.OnSearchSelectShow()\"\
     onmouseout=\"return searchBox.OnSearchSelectHide()\"\
     onkeydown=\"return searchBox.OnSearchSelectKey(event)\">\
<a class=\"SelectItem\" href=\"javascript:void(0)\" onclick=\"searchBox.OnSelectItem(0)\"><span class=\"SelectionMark\">&#160;</span>All</a>\
<a class=\"SelectItem\" href=\"javascript:void(0)\" onclick=\"searchBox.OnSelectItem(1)\"><span class=\"SelectionMark\">&#160;</span>Core</a>\
<a class=\"SelectItem\" href=\"javascript:void(0)\" onclick=\"searchBox.OnSelectItem(2)\"><span class=\"SelectionMark\">&#160;</span>Plugins</a>\
<a class=\"SelectItem\" href=\"javascript:void(0)\" onclick=\"searchBox.OnSelectItem(2)\"><span class=\"SelectionMark\">&#160;</span>User</a>\
<a class=\"SelectItem\" href=\"javascript:void(0)\" onclick=\"searchBox.OnSelectItem(2)\"><span class=\"SelectionMark\">&#160;</span>Public</a>\
<a class=\"SelectItem\" href=\"javascript:void(0)\" onclick=\"searchBox.OnSelectItem(2)\"><span class=\"SelectionMark\">&#160;</span>Private</a>\
</div>\
<!-- iframe showing the search results (closed by default) -->\
<div id=\"MSearchResultsWindow\">\
<iframe src=\"javascript:void(0)\" frameborder=\"0\"\
        name=\"MSearchResults\" id=\"MSearchResults\">\
</iframe></div></div>\
<section class=\"main-content\">\
<h1 class=\"title\">%s</h1><h2>Description</h2><em>%s</em>\n",
          doc->ctx->filename, doc->ctx->nspc->name, doc->ctx->tree->doc ? doc->ctx->tree->doc : "");

  fprintf(doc->html, "<h1>Global Types</h1>\n");
  for(i = 0; i < vector_size(doc->ctx->new_types); i++)
    mkdoc_type(doc, (Type)vector_at(doc->ctx->new_types, i));
  fprintf(doc->html, "</blockquote>\n");

  fprintf(doc->html, "<h2>Global Variables</h2><blockquote>\n");
  for(i = 0; i < vector_size(doc->ctx->new_values); i++)
    mkdoc_value(doc, ((Value)vector_at(doc->ctx->new_values, i)));
  fprintf(doc->html, "</blockquote>\n");

  fprintf(doc->html, "<h2>Global Functions</h2><blockquote>\n");
  for(i = 0; i < vector_size(doc->ctx->new_funcs); i++)
    mkdoc_func(doc, ((Func)vector_at(doc->ctx->new_funcs, i)));
  fprintf(doc->html, "</blockquote>\n");

  fprintf(doc->html, "</section>\n");
  fprintf(doc->html, "<footer class=\"site-footer\"><span class=\"site-footer-owner\"><a href=\"fennecdjay.github.io\">Gwion</a> is maintained by <a href=\"https://fennecdjay.github.io/Gwion/\">Jérémie Astor</a>.</span>\
  <span class=\"site-footer-credits\">This page was generated by <a href=\"https://pages.github.com\">GitHub Pages</a>, with a theme based on <a href=\"https://github.com/pietromenna/jekyll-cayman-theme\">Cayman theme</a> by <a\
href=\"http://github.com/jasonlong\">Jason Long</a>.</span>\
</footer></body></html>");
  free_Doc(doc);
}
