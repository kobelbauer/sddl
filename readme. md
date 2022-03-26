Description
-----------
Based on an ASTERIX recording, recorded air traffic surveillance data can be listed and converted to JSON.

Feature Summary
---------------
* Configurable compilation for optional listing or JSON writing
* Recording formats
  - asf     (ASTERIX in) IOSS Final Format recording
  - ioss    SASS-C IOSS (Final) recording (default)
  - net     Binary 'netto' recording
  - rec     Sequence of records
  - rff     Comsoft (TM) RFF recording
* ASTERIX categories
  - ASTERIX category 000	n.a.	April 1998
  - ASTERIX category 001	1.1 	August 2002
  - ASTERIX category 002	1.0 	November 1997
  - ASTERIX category 003	n.a.	April 1998
  - ASTERIX category 004	1.2 	March 2007
  - ASTERIX category 007	----	not supported
  - ASTERIX category 008	1.0 	November 1997
  - ASTERIX category 009	----	not supported
  - ASTERIX category 010	1.1 	March 2007
      option -vsn010=0.24s	0.24*	Sensis (Heathrow MDS modifications)
  - ASTERIX category 011	0.17	December 2001
      option -vsn011=0.14	0.14	October 2000
      option -vsn011=0.14i	0.14*	Sensis (Inn valley modification)
  - ASTERIX category 016	    	unknown
  - ASTERIX category 017	0.5	February 1999
  - ASTERIX category 018	----	not supported
  - ASTERIX category 019	1.1	March 2007
  - ASTERIX category 020	1.5	April 2008
      option -vsn020=1.0	1.0	November 2005
      option -vsn020=1.2	1.2	April 2007
      option -vsn020=1.5	1.5	April 2008
  - ASTERIX category 021	2.1	May 2011
      option -vsn021=0.12	0.12	February 2001
      option -vsn021=0.13	0.13	June 2001
      option -vsn021=0.20	0.20	December 2002
      option -vsn021=0.23	0.23	November 2003
      option -vsn021=1.0P	1.0P	April 2008
      option -vsn021=1.4	1.4	July 2009
      option -vsn021=2.1	2.1	May 2011
      option -vsn021=2.4	2.4	15 June 2015
  - ASTERIX category 023	1.2	March 2009
      option -vsn023=0.11	0.11	December 2002
      option -vsn023=1.0P	1.0P	April 2008
      option -vsn023=1.1	1.1	September 2008
      option -vsn023=1.2	1.2	March 2009
  - ASTERIX category 030	2.8.1	26 February 1999
  - ASTERIX category 031	2.8.1	26 February 1999
  - ASTERIX category 032	2.8.1	26 February 1999
  - ASTERIX category 034	1.27	May 2007
  - ASTERIX category 048	1.15	April 2007
      option -vsn048=1.14	1.14	November 2000
      option -vsn048=1.15	1.15	April 2007
      option -vsn048=1.16	1.16	March 2009
  - ASTERIX category 062	1.3	April 2005
  - ASTERIX category 063	1.0	March 2004
  - ASTERIX category 065	0.12	March 2003
      option -vsn065=0.12	0.12	March 2003
      option -vsn065=1.3	1.3	April 2007
  - ASTERIX category 221	?	?
  - ASTERIX category 247	1.2	February 2008
  - ASTERIX category 252	2.8.1	26 February 1999
* Configurable listing
* Configurable JSON writing
- none:         No JSON function (default)
- test:         Decode in JSON containers only, for benchmarks
- print:        Print JSON containers, e.g. for listing & verification
- text:         Write as JSON text file
- cbor:         Write as JSON CBOR (http://cbor.io/)
- msgpack:      Write as JSON MessagePack (https://msgpack.org/index.html)
- ubjson:       Write as JSON UBJSON (http://ubjson.org/)
- zip-text:     Write as ZIP archive containing a JSON text file
- zip-cbor:     Write as ZIP archive containing JSON CBOR
- zip-msgpack:  Write as ZIP archive containing JSON MessagePack
- zip-ubjson:   Write as ZIP archive containing UBJSON
 
For a complete list of the options, run "./bin/sddl -help"

Installation
------------
Required packages:
* g++
* cmake
* libarchive (for JSON writing, can be disabled)

Download or clone SDDL into a folder
Run "cmake ." in said folder
Run "make"
Successful compilations creates the sddl binary in folder "bin" and the library in "lib".

To disable listing for increased performance
* Disable listing by setting "#define LISTER     0" in "src/basics.h" and re-run make.
To remove the libarchive dependency and JSON functions
* Disable JSON by setting "set (USE_JSON false)" in CMakeLists.h and re-run cmake/make.

Contents
--------

Folder "src": Contains source code
CMakeLists.txt: CMake config file
gpl.txt: GPL license
readme.txt: This file

Authors
------
Helmut Kobelbauer
Contact: sddl@gmx.at

Helmut Puhr
Contact: atsdb@gmx.at

Licenses
--------

The source code is released under GNU GPLv3:
https://www.gnu.org/licenses/gpl-3.0.en.html

Please also note that the JSON functions are provided using:
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
|  |  |__   |  |  | | | |  version 3.1.2
|_____|_____|_____|_|___|  https://github.com/nlohmann/json
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2013-2018 Niels Lohmann <http://nlohmann.me>.

Disclaimer
----------

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


