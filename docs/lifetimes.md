# Lifetime Management Strategies

Because SimpleGL aims to be as lightweight as possible while also seamlessly integrating
with existing OpenGL codebases, adding elegant lifetime management is less straight forward
than you might think.

First, a look at the base case:

```c++
{
    sgl::GLResource<GL_ARRAY_BUFFER> buffer; // glGenBuffers has been called...
}
```

Here, the user has clearly made an error; a resource handle has been leaked. We could
take care of this for them by making sure to clean everything up in the destructor, but
we also want to provide the benefits of the library to GL resources that have been
created by other means:

```c++
void myFunction (GLuint programID) {
    sgl::Shader shader{programID}; // Here we simply wrap an existing resource.
    shader.setTexture(...);
}
```

If we simply released the resources after they left scope, this reasonable program
wouldn't work. Furthermore, simply using RAII would prevent us from passing around,
or copying `GLResource` objects. This means we can't have a `vector` or a `map` of GLResource.

There are a number of possible solutions to this, and I'll cover each of them briefly.

#### 1) Depend on runtime behavior
We could provide an additional argument to the constructor determining whether it
will be automatically managed or not:

```c++
{
    sgl::GLResource<GL_ARRAY_BUFFER> buffer{true}; // glGenBuffers is called
    sgl::GLResource<GL_TEXTURE_2D> texture; // glGenTextures is called
    sgl::GLResource<GL_FRAMEBUFFER> fbo{randomBool()};
} // buffer and maybe fbo is released
```

This approach is problematic because it's error prone, and makes code much harder to reason about.
A function receiving a `GLResource` has no way of reasoning about its lifetime, and whether its
actually freed might rely on hard to reason about logic.

#### 2) Factory

We could provide a factory class that generates `GLResource` instances and releases them all in its
destructor.

```c++
class Foo {
    ResourceManager manager;
    GLResource<GL_ARRAY_BUFFER> buf;

    Foo () :
        buf(manager.get())
    {}
};
```

This solution has many of the same problems as the first.



#### 3) Template Parameter
We could add an additional template parameter for `GLResource` that describes its lifetime
guarantees.

```c++
template <GLenum kind, bool managed=false>
class GLResource {
public:
    ~GLResource () {
        if (managed) release();
    }
};
```

This approach has the problem of infecting the rest of the types, adding more code, and a more
complicated interface for a feature that should be seamless.

#### 4) Resource Guard
We could provide a class similar to BindGuard that would handle releasing the gl object in
its destructor.
```c++
{
    GLResource<GL_ARRAY_BUFFER> buf;
    auto rg = resource_guard(buf);
    // ...
} // buf is released
```

This is a simple, non intrusive solution, but it buys us very little. If the user remembers to use
resource_guard they'd probably remember to use `GLResource::release()` as well. Furthermore, this
solution requires the user to come up with a variable name for each resource they'd like to free.

On the other hand, this does give us exception safety, and given the low cost may be worth including.
It also has the added benefit of working with subclasses of `GLResource`. Instead of eg: creating a
`ShaderM` class that duplicates all the code, we could define `using ShaderM = typename ResourceGuard<Shader>`.



