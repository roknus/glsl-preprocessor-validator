# GLSL preprocessor and validator

GLSL preprocessor and validator is a tool to generate GLSL shaders after preprocessing.

# 💡 Future ideas

* Add a validator options to see if the output shader is a valid in all OpenGL version (using glslangValidator)
* Add an option to output c++ header file with all the binding info

# ❓ Usage

```
Tool to preprocess and validate OpenGL shader files
Usage:
  glsl-preprocessor-validator [OPTION...] input shader file

      --help        Print help
  -p                Preprocess the shaders and output a file to the
                    [output] path
  -o, --output arg  Output file, only for preprocessor (default: "")
  -i, --input arg   Input file
```