# Snake Game for the Sinclair QL

## Object

This is a small sample application  which targets mainly an ancient machine, the
[Sinclair  QL](https://en.wikipedia.org/wiki/Sinclair_QL), but  it  can be  also
compiled for and played on any  Unix terminal with ncurses capability. Since the
QL was  my first  'professional' computer I  was interested to  find out  how it
compares to modern hardware and how it  feels to develop and compile software on
it in the 21st century.


## About the QL

The  QL was  announced  to the  public  in 1983  a few  weeks  before the  Apple
Macintosh. In this  time the machine was quite interesting.  Based on a Motorola
68008 it provided an internal  32bit CPU architecture, high resolution graphics,
two microdrive units with a storage capacity of 100 Kilobytes each and an office
software bundle  including text  processing, spreadsheet, business  graphics and
database. On contrast to the Macintosh and the IBM PC the QL was with an initial
price of  less then  £400 in  the UK very  affordable. Nevertheless  the machine
failed to sell well since it was delivered late, plagued with quality issues and
soon outperfomed  by more capable  machines like the  Atari ST and  the Comodore
Amgia  which came  with a  mouse controlled  graphical user  interface which  QL
lacked.

Despite its design flaws the machine offered some remarkable features. It comes
with a very compact and for its time advanced operating system which provides
e.g. preemptive multitaskting. It took the rest of the industry nearly a decade
to get that.

I  got  the QL  roughly  two  years after  its  announcement  and got  in  touch
with  more  serious  programming  languages  like  Forth  and  C.  Actually  the
[Lattice C Compiler](https://en.wikipedia.org/wiki/Lattice_C) from Metacomco was
the first  C Compiler  I used  and learned the  C-Programming language  with it.
Delivered  on three  microdrive  catriges  it was  incredible  slow  and it  was
definitely necessary  to buy a disc  drive in order  to actually use it  which I
did. Interestingly it was  possible to use this compiler on  a machine with less
than 96K of free memory!

### C-Compilers for the QL

#### Lattice-C from Metacomco

The Lattice  C compiler is unfortunately  not free software and  obviously still
used on some mainframe  systems so it is not available on the  web. Since I sold
my original QL with my complete software  library included, I do not have access
to this compiler  at moment. Hopefully a  friend my mine will help  me out later
this year.

#### C68

As an  alternative, with C68  there is free  and more ANSI-C  compliant compiler
available  which was  developed  by Dave  Walker  in 1997.  It  can be  obtained
from the  [Sinclair QL  website of Dilwyn  Jones](http://www.dilwyn.me.uk) which
provides an quite extensive collection of documentation and software for the QL.
C68 runs exclusively on an QL with  the Toolkit II extension from Tony Tebby and
an  exteneded  memory  to  at  least  640KB  which  is  the  'official'  maximum
configuration. Even with that the combination of the build menu and the compiler
itself causes out  of memory errors but  it is at least possible  to compile and
link a  small application such  as snake with a  basic compile script  which you
find included in this package.

#### QDOS-GCC

Another option is to cross compile C  programs for the QL with QDOS-GCC which is
basically a  patched version of  the GNU-C compiler.  GNU-C is considered  to be
superiour to C68 in terms of build and run time efficiency, standard compliance,
etc. The  QDOS-GCC version however  is based on GCC  2.95 which is  from today's
perspective old as the hills. It is said to be impossible to compile it with any
modern  version of  GCC. I  found out  the easiest  way to  get QDOS-GCC  up and
running is to build it on a virtual machine running a Linux distribution of that
age.  I  used  [GNU/Debian-Woody](https://www.debian.org/releases/woody)  pretty
successfully for that.  The provided Makefile allows the  invocation of QDOS-GCC
with some additional configuration arguments.


## Compiling Snake

This repository provides  configuration files for the autotools  build system in
order compile or cross compile with a GNU Compiler. Furthermore for illustration
purposes the software can be compiled  on a physical or simulated QL environment
as well. It is desperately slow but it works. The following sections explain how
to do this.

### Compile the Unix Version

Compiling for Unix/GNU-Linux  is actually most simple and  straigfoward. Use the
following commands to configure, compile and install snake on your host machine:

    autoreconf -i
    ./configure
    make
    make install

The last command installs the  snake executable binary file under /usr/local/bin
and requires root  credentials. Make sure that you have  the development version
of the ncurses library installed on your  system. Under debian this is done with
the command

    apt-get install libncurses5-dev


### Cross Compilation for the Sinclair QL using qdos-gcc

Cross compilation requires to compile the  QDOS-GCC compiler first. Refer to the
[website of Thierry Godefroy](http://morloch.hd.free.fr/qdos/qdosgcc.html) about
howto to do this. As already mentioned  the easiest way seems to install a Linux
distribution  which already  comes  with the  required GCC  version  2.95 as  in
example Debian-Woody. The following instructions are based on that approach.

Debian Woody  comes with a very  ancient versions of the  autotools build script
frame work. Luckily non  of the core features are really  missing. What you need
to do  is to create some  standard documentation files required  in GNU software
since the option  to disable to check  them is not available  Woody's version of
autoconf. Create these with the following command:

    touch NEWS AUTHORS ChangeLog

The configure script is generated as usual with the following command. Make sure
that you have no artefacts from previous builds in your directory.

     autoreconf -i

Then configure the build system and  start cross compilation. This requires that
qdos-gcc is accissible from your install path:

    ./configure --build=i386-pc-linux-gnu --host=m68k-coff --enable-qdos CC=qdos-gcc
    make

Be aware that you need to add an  additional header of 30 bytes to the resulting
snake executable  file which is required  by the QDOS commands  exec and exec_w.
Since the header seems to be more or  less the same for every executable of that
type you can copy  over the initial 30 bytes from  any other function executable
and prepend  them to  your snake binary.  You will also  find a  small C-utility
program for adding the QL binary header under the name addqlh.c.


### Compile the Sources under QDOS

The most  unpractical but neverless interesing experience is how it feels like to
compile this primitive application on the original hardware. QDOS comes with an
unique naming scheme for file names where the file name and the file extension are
separated by an underline character. So you need to rename all .c files to _c and
.h to _h since otherwise the compiler is not able to locate any source files. You
find a shell script which applies the rename operation for all source files which
is invoked by the following command:

    ./rename-sources-for-ql.sh

Afterwards copy over all  source files to your QL. If you have  an IDE hard disk
interface like QUBIDE, you can use the
[Image Editor Utitily](http://hardware.speccy.org/temp/qubide4-i.html) provided
by habisoft for this.

To compile  snake on the QL  first load the basic  program 'build_snake_bas' and
set the default directories for the C68  program files and the data source files
of snake  to the corresponding directories.  The compilation is started  by just
starting this basic  program with 'run'. The original hardware  it is incredible
slow and  much probably  there will  be an out  of memory  error in  the linkage
phase. The problem can be addressed by  reducing the maximum allowed length of C
specifiers in the  C68 linker source files and recompiling  them. If you managed
to get to this stage I can provide  you a patched version of the linker as well.
Just write me an email.


## Executing the Snake Game

On the QL the  snake game is started by the command exec_w  followed by the full
qualified file name of the snake binary (snake or snake_exe).

The  computer plays  admittedly a  little bit  stupid. There  are for  sure much
better ways to  implement a more clever  computer player but do  not forget that
the QL platform is  quite limited and the ultimate goal is of  course to run the
application even on a  standard QL with 128KB of RAM. Anyway, I  am open for any
suggestions for improvement.


## License

This implementation code stands under the terms of the
[GNU GENERAL PUBLIC LICENSE 2.0](http://www.gnu.org/licenses/old-licenses/gpl-2.0).

January 2017, Otto Linnemann


## Resources and links

Thanks to all the  people who helped to keep the QL  platform alive and provided
documentation and software for it:

* [Dilwyn Jones for the Sinclair QL Pages](http://www.dilwyn.me.uk)
* [Daniele Terdina for Q-emuLator](http://terdina.net/ql/q-emulator.html) and the
* [Sinclair QL: the Italien Preservation](https://sinclairql.wordpress.com)
* [Urs König for the Sinclair QL Preservation Project](http://www.sinclairql.net/repository.html)
* [Dave Walker for developing C68](http://web.archive.org/web/20040606145627/http://homepage.ntlworld.com/itimpi/qdosmenu.htm)
* [Richard Zidlicky, Jonathan Hudson, Thierry Godefroy and Dave Walker for QDOS-GCC](http://morloch.hd.free.fr/qdos/qdosgcc.html)