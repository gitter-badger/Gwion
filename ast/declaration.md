---
layout: page
title:	Declaration
categories: exp
---
  <h1>Declaration</h1>
Declaration are very simple:
```c
<your type> var1;
<your type2> var2, var3;
```

a variable can be declared as ref: it won't be initialized,
but it can be used to store refence to object of the same type.
a primitive can't be declared as ref.
```c
Object o;
Object @ref;
// store reference to o in ref;
o @=> ref;
// print both objects
<<<o, ref>>>;
```

you can also declare **Arrays**:
```c
// a array 3 int
int i[3];
// empty [] make ref;
Object array[];
// is similar to
Object @array2[];
// array can be multi-dimensionnal
float array3d[3]4][5];
```
