---
layout: default
title:  "Template"
categories: features
---

You can declare a template function:

```c
template<{your typenames}>
function ret_type function_names(argumens)
{
	// your code here
}	
```

```lua
function test() {
  console.log("notice the blank line before this function?");
}
```
```ruby
function test() {
  console.log("notice the blank line before this function?");
}
```
{% highlight javascript %}
{% include alert_msg.js %}
{% endhighlight %}

```ruby
require 'tabbit'
table = Tabbit.new('Name', 'Email')
table.add_row('Tim Green', 'tiimgreen@gmail.com')
puts table.to_s
```

~~~ruby
require 'tabbit'
table = Tabbit.new('Name', 'Email')
table.add_row('Tim Green', 'tiimgreen@gmail.com')
puts table.to_s
~~~

```ruby
require 'redcarpet'
markdown = Redcarpet.new("Hello World!")
puts markdown.to_html
```
