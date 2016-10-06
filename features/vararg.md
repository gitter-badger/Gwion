---
layout: default
title:  "variadic functions"
date:   2016-10-12 17:50:00
categories: features
---

variadic function take a set of predefined arguments, plus any number of any type of arguments.  
they are declared this way:
{% highlight c %}
variadic void func_name(set of arguments)
{% endhighlight %}
you can then loop through args:
>	vararg.start
>	// more code...
>	vararg.end

acces is done with:
>	vararg.i // for int  
>	vararg.f // for float
>	vararg.c // for complex TODO: polar
>	vararg.o // for object

**vararg.o** can be casted to any object type.
