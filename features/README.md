---
layout: homepage
title: {{ site.name }}
---

<div id="home">
<div id="header">
  gwion is a object-oriented programming language, aimed at making music<br>
</div>
<b>strongly</b> inspired by <a href="http://chuck.stanford.edu/">chuck</a><br>
synthesis is based on <a href="http://paulbatchelor.github.io/proj/soundpipe.html">soundpipe</a>

<blockquote><p>
// create a sinusoidal generator and connect it to dac </br>
SinOsc s => dac;<br>
// let one minute pass<br>
minute => now;<br>
// you're done!<br>
</p></blockquote>
  <h1>Features</h1>
  <ul class="posts">
    {% for post in site.pages %}
      {% if post.categories == "features" %}
        <li><span><a href="{{ site.baseurl }}{{ post.url }}">{{post.title}}</a></li>
      {% endif %}
    {% endfor %}
  </ul>
  <h1>Examples</h1>
  <ul class="posts">
    {% for post in site.pages %}
      {% if post.categories == "examples" %}
        <li><span><a href="{{ site.baseurl }}{{ post.url }}">{{post.title}}</a></li>
      {% endif %}
    {% endfor %}
  </ul>
  <h1>Plugins</h1>
  <ul class="posts">
    {% for post in site.pages %}
      {% if post.categories == "plugins" %}
        <li><span><a href="{{ site.baseurl }}{{ post.url }}">{{post.title}}</a></li>
      {% endif %}
    {% endfor %}
  </ul>
</div>