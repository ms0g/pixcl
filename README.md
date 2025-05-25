# pixcl
A fast, cross-platform command-line tool for real-time image processing using OpenCL acceleration.

<img src="assets/lenna.png" alt="image" width="402" height="auto"> <img src="assets/lenna_gb.png" alt="image" width="402" height="auto">
<img src="assets/lenna_gs.png" alt="image" width="402" height="auto"> <img src="assets/lenna_sep.png" alt="image" width="402" height="auto">

## Usage
```bash
➜  ~ pixcl -h
OVERVIEW: An OpenCL-based image processing tool 
 
USAGE: pixcl [options] <image file>

OPTIONS:
  -e  --effect          Effect to be applied[gb/gs/sep]
  -f, --format          File format[jpg <quality 0-100>?/png/bmp/tga/raw]
  -o, --outfile         Output file name
  -h, --help            Display available options
  -v, --version         Display the version of this program
```
```bash
➜  ~ pixcl lenna.png -e gb -f png -o out.png
```
## License
This project is licensed under the BSD 3-Clause License. See the LICENSE file for details.