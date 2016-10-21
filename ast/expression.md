---
layout: page
title:	Expressions
categories: ast
---
  <h1>Expressions</h1>
here we document possible expressions
<ul class="posts">
	{% for post in site.pages %}
		{% if post.categories == "exp" %}
			<li><a href="{{ site.baseurl }}{{ post.url }}">{{post.title}}</a></li>
		{% endif %}
	{% endfor %}
</ul>
