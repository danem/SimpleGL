# TODO

* ~~Add some compile time opengl version detection magic~~
* ~~Add support for GLES. This should be a matter of feature detection.~~
* Support Program Pipelines
* Figure out an interface for generating and updating textures / renderbuffers
* Rework GLBuffer API. Also consider storing buffer size along with the resource itself
* Reconsider GLResource creation. Allocating a GLuint handle in the constructor can make it easy to accidentally leak resources.

