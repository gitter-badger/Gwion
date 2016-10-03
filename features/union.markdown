---
layout: default
title:  "Union"
date:   2016-10-12 17:50:00
categories: features
---

Gwion features unamed union.  
declared object are refs

at global scope:
> union 
> {
> 	int     i;
> 	float   f;
> 	complex c;
> 	Object  o;
> };


same at class scope:
> class C
>	{
>		union 
> 	{
> 		int     i;
> 		float   f;
> 		complex c;
> 		Object  o;
> 	};
>	}
