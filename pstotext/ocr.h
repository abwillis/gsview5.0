
RCDATA 1
BEGIN
  "% Copyright (C) 1995, Digital Equipment Corporation.\012"
  "% All rights reserved.\012"
  "% See the file pstotext.txt for a full description.\012"
  "\012"
  "% ocr.ps, part of BuildLectern\012"
  "%\012"
  "%   This is a PostScript library to send the characters rendered by a\012"
  "%   PostScript job back on stdout.  The output is intended to allow\012"
  "%   reconstruction of the document's words and an approximation of the words'\012"
  "%   bounding rectangles.\012"
  "\012"
  "% Last modified on Fri Jan  9 21:20:00 AEST 2004 by rjl\012"
  "%      modified on Sat Feb  5 21:00:00 AEST 2000 by rjl\012"
  "%      modified on Fri Oct  2 17:13:53 PDT 1998 by mcjones\012"
  "%      modified on Thu Jan 25 15:24:37 PST 1996 by deutsch\012"
  "%      modified on Wed May  3 15:41:30 PDT 1995 by birrell\012"
  "\012"
  "% Modified by rjl\012"
  "%   For Ghostscript 6 or later.\012"
  "%   Change ashow, widthshow and awidthshow to call show for each character.\012"
  "%   Add xshow, yshow, xyshow.\012"
  "\012"
  "% Restrictions:\012"
  "%\012"
  "%   This library redefines some names that are originally defined as\012"
  "%   operators, and of course the new definitions are procedures. Some\012"
  "%   jobs might be sensitive to this distinction.  In this case, you'd\012"
  "%   need to make \042redef\042 create a new operator.  For example, \042odef\042.\012"
  "%\012"
  "%   This library reports characters rendered by show, ashow, widthshow,\012"
  "%   awidthshow, kshow, xshow, yshow and xyshow.  \012"
  "%   The characters are reported even if they would\012"
  "%   be invisible in final hardcopy.  This could happen, for example, if the\012"
  "%   characters get clipped, or if they get overprinted, or if they are the\012"
  "%   same color as their background, or if the font's glyph make no marks.\012"
  "%\012"
  "%   PostScript doesn't specify a standard character code for the characters\012"
  "%   being rendered; rather, the font maps small integers into glyphs.  It's\012"
  "%   not possible in general to determine a translation back into a standard\012"
  "%   character code.  This library approximates this by giving mappings from\012"
  "%   the job's characters to indexes into a table of known glyph names.  This\012"
  "%   handles most PostScript jobs that we've encountered, but it's not an\012"
  "%   absolute solution to the problem.\012"
  "\012"
  "% Output format:\012"
  "%\012"
  "%    The \042reporting coordinate system\042 is the device coordinate system.  The\012"
  "%    application interpreting this library's output needs to understand the\012"
  "%    device coordinate system.\012"
  "%\012"
  "%    Positions are reported in the reporting coordinate system, but with (x,y) \012"
  "%    values multiplied by 100 and rounded to integers.  Note that\012"
  "%    positions might involve negative integers.\012"
  "%\012"
  "%    The directives, whitespace and numbers are themselves encoded in ASCII.\012"
  "%    The strings in the \042S\042 directives are in currentfont's encoding: they\012"
  "%    should be treated as 8-bit binary data.\012"
  "%\012"
  "%    QI 6-integers\012"
  "%                       Specifies the inverse of the currentmatrix for\012"
  "%                       the current output device, after rotXXX.ps and\012"
  "%                       before the document is processed.\012"
  "%\012"
  "%    QM m blx bly trx try 256-pairs\012"
  "%			Introduces a new character metrics table used by\012"
  "%			some font, where \042m\042 is an integer that identifies\012"
  "%			the metrics table in subsequent \042F\042 directives.  \042m\042\012"
  "%			is small.  (blx,bly) is the bottom left corner of the\012"
  "%			font's bounding box, and (trx,try) is its top right. \012"
  "%			The pairs are the stringwidth of each character in the\012"
  "%			font's encoding.  All of these are reported in the\012"
  "%			font's character coordinate system, multiplied by 100\012"
  "%			and rounded to integers.  The metrics\012"
  "%			table will be referenced in a subsequent \042F\042\012"
  "%			directive, which includes information mapping the\012"
  "%			font's metrics to the initial user coordinate system. \012"
  "%			Note that values of \042m\042 might get reused, after a\012"
  "%			\042restore\042.\012"
  "%\012"
  "% QE e n n-integers	Introduces a new encoding, where \042e\042 is an integer\012"
  "%			that identifies the encoding in subsequent \042F\042\012"
  "%			directives.  \042e\042 is small.  \042e\042 is followed by\012"
  "%			an integer \042n\042 and then by a sequence of\012"
  "%			exactly n integers.  The i'th integer\012"
  "%			specifies the glyph for the i'th entry of the\012"
  "%			font's encoding vector.  The value of the\012"
  "%			integer is usually an index in the array\012"
  "%			\042StandardGlyphs\042, defined below.  Note that\012"
  "%			the first 256 entries of that array equal the\012"
  "%			corresponding entries of ISOLatin1Encoding.\012"
  "%			The value 9999 indicates that this entry in\012"
  "%			the font's encoding specifies a glyph not\012"
  "%			named in StandardGlyphs.  Note that values of\012"
  "%			\042e\042 might get reused, after a \042restore\042.\012"
  "%\012"
  "%    QF n x y x' y' e m	Introduces a new font, where \042n\042 is an integer\012"
  "%			that identifies the font in subsequent \042S\042\012"
  "%			directives.  \042n\042 is small.  (x,y) is the\012"
  "%			position corresponding to (1000,0) in the font's\012"
  "%			character coordinate system, and (x',y') is the\012"
  "%			position corresponding to (0,1000), both as they would\012"
  "%			be if the character were drawn with its origin at\012"
  "%			the origin of the reporting coordinate system. \042e\042 is\012"
  "%			an integer specifying a previously defined\012"
  "%			encoding vector.  \042m\042 is an integer specifying a\012"
  "%			previously defined metrics table.  Note that values\012"
  "%			of \042n\042 might get reused, after a \042restore\042.\012"
  "%\012"
  "%    QS n x y l s x' y'	Reports rendering of string \042s\042 in font \042n\042.\012"
  "%			(x,y) is the position corresponding to the origin of\012"
  "%			the first character. \042l\042 is length of the string,\012"
  "%			followed immediately by a single space then the string.\012"
  "%			(x',y') is the position that would correspond to the\012"
  "%			origin of a subsequent character. The\012"
  "%			string reported by this directive is never empty.\012"
  "%			The string also never contains a \042space\042: strings\012"
  "%			that would have contained a \042space\042 are split up\012"
  "%			into multiple directives, with the \042space\042\012"
  "%			omitted.  Here \042space\042 means the first character\012"
  "%			in the font's encoding that maps to the glyph\012"
  "%			named \042/space\042, if there is such a character.\012"
  "%\012"
  "%    QC			copypage was invoked\012"
  "%\012"
  "%    QZ			erasepage was invoked\012"
  "%\012"
  "%    QP			showpage was invoked\012"
  "\012"
  "%\012"
  "% globals and subroutines\012"
  "%\012"
  "\012"
  "%/setglobal where\012"
  "% { pop currentglobal /setglobal load true setglobal }\012"
  "% { { } }\012"
  "%ifelse\012"
  "revision 353 ge {\012"
  "NOBIND /DELAYBIND where { pop DELAYBIND or } if\012"
  " { systemdict begin\012"
  "                /bind /.bind load\012"
  "                  /.forcedef where { pop .forcedef } { def } ifelse\012"
  "              end\012"
  " }\012"
  "if\012"
  "} if\012"
  "\012"
  "% put our private stuff in a local dictionary,\012"
  "% but place a reference to it in systemdict\012"
  "systemdict begin\012"
  "/pstotextLocalDict 30 dict /.forcedef where { pop .forcedef } { def } ifelse\012"
  "end\012"
  "\012"
  "pstotextLocalDict begin		% following stuff is private\012"
  "/redef { systemdict begin 1 index exch .makeoperator def end } bind def\012"
  "/privateDict currentdict def	% for lastFontNum and lastEncoding\012"
  "/fonts 200 dict def		% maps font to integer \042n\042\012"
  "/fontsUnit 200 dict def 	% (1000,0) and (0,1000) transformed, per font\012"
  "/encodings 200 dict def		% maps encoding array to integer \042e\042\012"
  "/encodingSpace 200 dict def	% space char for each encoding\012"
  "/metrics 200 dict def		% maps font UniqueID to integer  \042m\042\012"
  "/lastFontNum 0 def		% last integer used for a font\012"
  "/lastEncoding 0 def		% last integer used for an encoding\012"
  "/lastMetrics 0 def		% last integer used for a metrics table\012"
  "/tempString 20 string def	% scratch for printing integers\012"
  "/reportMatrix matrix identmatrix def % maps device coords to reporting coords\012"
  "/inUse false def		% prevents recursive invokcation of \042report\042\012"
  "\012"
  "/TimesRomanGlyphs [\012"
  "  % ISOLatin1Encoding ...\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /space /exclam /quotedbl\012"
  "  /numbersign /dollar /percent /ampersand /quoteright\012"
  "  /parenleft /parenright /asterisk /plus /comma\012"
  "  /minus /period /slash /zero /one\012"
  "  % 50\012"
  "  /two /three /four /five /six\012"
  "  /seven /eight /nine /colon /semicolon\012"
  "  /less /equal /greater /question /at\012"
  "  /A /B /C /D /E\012"
  "  /F /G /H /I /J\012"
  "  /K /L /M /N /O\012"
  "  /P /Q /R /S /T\012"
  "  /U /V /W /X /Y\012"
  "  /Z /bracketleft /backslash /bracketright /asciicircum\012"
  "  /underscore /quoteleft /a /b /c\012"
  "  % 100\012"
  "  /d /e /f /g /h\012"
  "  /i /j /k /l /m\012"
  "  /n /o /p /q /r\012"
  "  /s /t /u /v /w\012"
  "  /x /y /z /braceleft /bar\012"
  "  /braceright /asciitilde /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /.notdef\012"
  "  /.notdef /.notdef /.notdef /.notdef /dotlessi\012"
  "  /grave /acute /circumflex /tilde /macron\012"
  "  % 150\012"
  "  /breve /dotaccent /dieresis /.notdef /ring\012"
  "  /cedilla /.notdef /hungarumlaut /ogonek /caron\012"
  "  /space /exclamdown /cent /sterling /currency\012"
  "  /yen /brokenbar /section /dieresis /copyright\012"
  "  /ordfeminine /guillemotleft /logicalnot /hyphen /registered\012"
  "  /macron /degree /plusminus /twosuperior /threesuperior\012"
  "  /acute /mu /paragraph /periodcentered /cedilla\012"
  "  /onesuperior /ordmasculine /guillemotright /onequarter /onehalf\012"
  "  /threequarters /questiondown /Agrave /Aacute /Acircumflex\012"
  "  /Atilde /Adieresis /Aring /AE /Ccedilla\012"
  "  % 200\012"
  "  /Egrave /Eacute /Ecircumflex /Edieresis /Igrave\012"
  "  /Iacute /Icircumflex /Idieresis /Eth /Ntilde\012"
  "  /Ograve /Oacute /Ocircumflex /Otilde /Odieresis\012"
  "  /multiply /Oslash /Ugrave /Uacute /Ucircumflex\012"
  "  /Udieresis /Yacute /Thorn /germandbls /agrave\012"
  "  /aacute /acircumflex /atilde /adieresis /aring\012"
  "  /ae /ccedilla /egrave /eacute /ecircumflex\012"
  "  /edieresis /igrave /iacute /icircumflex /idieresis\012"
  "  /eth /ntilde /ograve /oacute /ocircumflex\012"
  "  /otilde /odieresis /divide /oslash /ugrave\012"
  "  % 250\012"
  "  /uacute /ucircumflex /udieresis /yacute /thorn\012"
  "  /ydieresis \012"
  "  % Other glyphs from /Times-Roman ...\012"
  "  % 256\012"
  "  /quotedblright /Scaron /dagger /guilsinglleft\012"
  "  /Zcaron /daggerdbl /Lslash /ellipsis /guilsinglright\012"
  "  /oe /fi /bullet /perthousand /quotedblbase\012"
  "  /endash /emdash /trademark /florin /lslash\012"
  "  /scaron /Ydieresis /fl /fraction /quotedblleft\012"
  "  /quotesinglbase /quotesingle /zcaron /OE\012"
  "  % 284\012"
  "  ] def\012"
  "\012"
  "/dvipsGlyphs [\012"
  "  % Self-named glyphs for dvitops type 3 fonts ...\012"
  "  % 284\012"
  "  0 1 255 { 1 string dup 0 4 -1 roll put cvn } for\012"
  "  % 540\012"
  "  ] def\012"
  "\012"
  "/ttypeGlyphs1 [\012"
  "  % Glyph names used by MS TrueType encodings ...\012"
  "  % 540\012"
  "  /G00 /G01 /G02 /G03 /G04 /G05 /G06 /G07 /G08 /G09 /G0a /G0b /G0c /G0d /G0e\012"
  "  /G0f /G10 /G11 /G12 /G13 /G14 /G15 /G16 /G17 /G18 /G19 /G1a /G1b /G1c /G1d\012"
  "  /G1e /G1f /G20 /G21 /G22 /G23 /G24 /G25 /G26 /G27 /G28 /G29 /G2a /G2b /G2c\012"
  "  /G2d /G2e /G2f /G30 /G31 /G32 /G33 /G34 /G35 /G36 /G37 /G38 /G39 /G3a /G3b\012"
  "  % 600\012"
  "  /G3c /G3d /G3e /G3f /G40 /G41 /G42 /G43 /G44 /G45 /G46 /G47 /G48 /G49 /G4a\012"
  "  /G4b /G4c /G4d /G4e /G4f /G50 /G51 /G52 /G53 /G54 /G55 /G56 /G57 /G58 /G59\012"
  "  /G5a /G5b /G5c /G5d /G5e /G5f /G60 /G61 /G62 /G63 /G64 /G65 /G66 /G67 /G68\012"
  "  /G69 /G6a /G6b /G6c /G6d /G6e /G6f /G70 /G71 /G72 /G73 /G74 /G75 /G76 /G77\012"
  "  /G78 /G79 /G7a /G7b /G7c /G7d /G7e /G7f /G80 /G81 /G82 /G83 /G84 /G85 /G86\012"
  "  /G87 /G88 /G89 /G8a /G8b /G8c /G8d /G8e /G8f /G90 /G91 /G92 /G93 /G94 /G95\012"
  "  /G96 /G97 /G98 /G99 /G9a /G9b /G9c /G9d /G9e /G9f /Ga0 /Ga1 /Ga2 /Ga3 /Ga4\012"
  "  /Ga5 /Ga6 /Ga7 /Ga8 /Ga9 /Gaa /Gab /Gac /Gad /Gae /Gaf /Gb0 /Gb1 /Gb2 /Gb3\012"
  "  /Gb4 /Gb5 /Gb6 /Gb7 /Gb8 /Gb9 /Gba /Gbb /Gbc /Gbd /Gbe /Gbf /Gc0 /Gc1 /Gc2\012"
  "  /Gc3 /Gc4 /Gc5 /Gc6 /Gc7 /Gc8 /Gc9 /Gca /Gcb /Gcc /Gcd /Gce /Gcf /Gd0 /Gd1\012"
  "  % 750\012"
  "  /Gd2 /Gd3 /Gd4 /Gd5 /Gd6 /Gd7 /Gd8 /Gd9 /Gda /Gdb /Gdc /Gdd /Gde /Gdf /Ge0\012"
  "  /Ge1 /Ge2 /Ge3 /Ge4 /Ge5 /Ge6 /Ge7 /Ge8 /Ge9 /Gea /Geb /Gec /Ged /Gee /Gef\012"
  "  /Gf0 /Gf1 /Gf2 /Gf3 /Gf4 /Gf5 /Gf6 /Gf7 /Gf8 /Gf9 /Gfa /Gfb /Gfc /Gfd /Gfe\012"
  "  /Gff \012"
  "  % 796\012"
  "  ] def\012"
  "\012"
  "/ttypeGlyphs2 [\012"
  "  % 796\012"
  "  /G00 /G01 /G02 /G03 /G04 /G05 /G06 /G07 /G08 /G09 /G0A /G0B /G0C /G0D\012"
  "  /G0E /G0F /G10 /G11 /G12 /G13 /G14 /G15 /G16 /G17 /G18 /G19 /G1A /G1B /G1C\012"
  "  /G1D /G1E /G1F /G20 /G21 /G22 /G23 /G24 /G25 /G26 /G27 /G28 /G29 /G2A /G2B\012"
  "  /G2C /G2D /G2E /G2F /G30 /G31 /G32 /G33 /G34 /G35 /G36 /G37 /G38 /G39 /G3A\012"
  "  /G3B /G3C /G3D /G3E /G3F /G40 /G41 /G42 /G43 /G44 /G45 /G46 /G47 /G48 /G49\012"
  "  /G4A /G4B /G4C /G4D /G4E /G4F /G50 /G51 /G52 /G53 /G54 /G55 /G56 /G57 /G58\012"
  "  /G59 /G5A /G5B /G5C /G5D /G5E /G5F /G60 /G61 /G62 /G63 /G64 /G65 /G66 /G67\012"
  "  % 900\012"
  "  /G68 /G69 /G6A /G6B /G6C /G6D /G6E /G6F /G70 /G71 /G72 /G73 /G74 /G75 /G76\012"
  "  /G77 /G78 /G79 /G7A /G7B /G7C /G7D /G7E /G7F /G80 /G81 /G82 /G83 /G84 /G85\012"
  "  /G86 /G87 /G88 /G89 /G8A /G8B /G8C /G8D /G8E /G8F /G90 /G91 /G92 /G93 /G94\012"
  "  /G95 /G96 /G97 /G98 /G99 /G9A /G9B /G9C /G9D /G9E /G9F /GA0 /GA1 /GA2 /GA3\012"
  "  /GA4 /GA5 /GA6 /GA7 /GA8 /GA9 /GAA /GAB /GAC /GAD /GAE /GAF /GB0 /GB1 /GB2\012"
  "  /GB3 /GB4 /GB5 /GB6 /GB7 /GB8 /GB9 /GBA /GBB /GBC /GBD /GBE /GBF /GC0 /GC1\012"
  "  /GC2 /GC3 /GC4 /GC5 /GC6 /GC7 /GC8 /GC9 /GCA /GCB /GCC /GCD /GCE /GCF /GD0\012"
  "  /GD1 /GD2 /GD3 /GD4 /GD5 /GD6 /GD7 /GD8 /GD9 /GDA /GDB /GDC /GDD /GDE /GDF\012"
  "  /GE0 /GE1 /GE2 /GE3 /GE4 /GE5 /GE6 /GE7 /GE8 /GE9 /GEA /GEB /GEC /GED /GEE\012"
  "  /GEF /GF0 /GF1 /GF2 /GF3 /GF4 /GF5 /GF6 /GF7 /GF8 /GF9 /GFA /GFB /GFC /GFD\012"
  "  % 1050\012"
  "  /GFE /GFF \012"
  "  % 1052\012"
  "  ] def\012"
  "\012"
  "/oldDviGlyphs [\012"
  "  % More self-named glyphs for old dvitops type 3 fonts ...\012"
  "  % 1052\012"
  "  0 1 127 { 10 3 string cvrs cvn } for\012"
  "  % 1180\012"
  "  ] def\012"
  "\012"
  "/StandardGlyphs //TimesRomanGlyphs length 896 add array def\012"
  "//StandardGlyphs 0 //TimesRomanGlyphs putinterval\012"
  "//StandardGlyphs //TimesRomanGlyphs length //dvipsGlyphs putinterval\012"
  "//StandardGlyphs //TimesRomanGlyphs length 256 add //ttypeGlyphs1 putinterval\012"
  "//StandardGlyphs //TimesRomanGlyphs length 512 add //ttypeGlyphs2 putinterval\012"
  "//StandardGlyphs //TimesRomanGlyphs length 768 add //oldDviGlyphs putinterval\012"
  "\012"
  "/standardMap StandardGlyphs length dict\012"
  "  % Maps names to indices in StandardGlyphs.\012"
  "  0 StandardGlyphs {\012"
  "    2 index exch\012"
  "    2 copy known { pop pop } { 2 index put } ifelse\012"
  "    1 add\012"
  "    } forall\012"
  "  pop def\012"
  "\012"
  "/printInt {			% stack: n\012"
  "  % Prints an integer followed by a space on stdout\012"
  "  //tempString cvs print ( ) print\012"
  "  } bind def\012"
  "\012"
  "/showxy {			%stack:  x y\012"
  "  % prints a pair of integers on stdout, converting to 1/100th's\012"
  "  exch\012"
  "  100 mul round cvi //printInt exec\012"
  "  100 mul round cvi //printInt exec\012"
  "  } bind def\012"
  "\012"
  "/characterToReporting {		% stack: x y -> x' y'\012"
  "  % Transforms a vector in currentfont's character coordinate\012"
  "  % system to the reporting coordinate system\012"
  "  currentfont /FontMatrix get\012"
  "  dtransform			% to current user coordinates\012"
  "  dtransform			% to device coordinates\012"
  "  //reportMatrix idtransform	% to reporting coordinates\012"
  "  } def\012"
  "\012"
  "/printCharacterOrigin {\012"
  "  % Prints the position in the reporting coordinate system at which a\012"
  "  % character origin would be painted using currentfont\012"
  "  0 0 currentfont /FontMatrix get transform\012"
  "  currentpoint exch 4 -1 roll add 3 1 roll add\012"
  "  transform //reportMatrix itransform\012"
  "  //showxy exec\012"
  "  } bind def\012"
  "\012"
  "/printMap {\012"
  "  % Print map from indices in currentfont/Encoding to StandardEncoding indices\012"
  "  16 currentfont /Encoding get dup length //printInt exec {\012"
  "    exch dup 16 eq { () = pop 1 } { 1 add } ifelse exch\012"
  "    //standardMap exch\012"
  "    2 copy known {\012"
  "      get //printInt exec\012"
  "      } {\012"
  "      pop pop (9999 ) print\012"
  "      } ifelse\012"
  "    } forall\012"
  "  pop () =\012"
  "  } bind def\012"
  "\012"
  "/metricsString (X) def\012"
  "\012"
  "/printMetrics {\012"
  "  % Print bounding box and character metrics for currentfont\012"
  "  % Sadly, dvitops produces illegal type 3 fonts with no /.notdef entry. The\012"
  "  % use of \042stopped\042 deals with that and any other silliness.\012"
  "  currentfont /FontBBox get aload pop 4 2 roll\012"
  "  //showxy exec\012"
  "  //showxy exec\012"
  "  currentfont /FontMatrix get\012"
  "  currentfont /FontType get 3 eq {\012"
  "    //privateDict /pm.dictCount countdictstack put\012"
  "    count //privateDict exch /pm.count exch put\012"
  "    //privateDict /pm.save save put\012"
  "    0 1 255 {\012"
  "      dup 8 mod 0 eq { () = } if\012"
  "      //metricsString exch 0 exch put\012"
  "      { //metricsString //stringwidth exec\012"
  "        2 index idtransform		% to font's character coordinates\012"
  "        } stopped {\012"
  "        count //privateDict /pm.count get sub { pop } repeat\012"
  "        countdictstack //privateDict /pm.dictCount get sub { end } repeat\012"
  "        //privateDict /pm.save get restore\012"
  "        //privateDict /pm.save save put\012"
  "        0 0\012"
  "        } if\012"
  "      //showxy exec\012"
  "      } for\012"
  "    //privateDict /pm.save get restore % necessary to balance the \042save\042\012"
  "    }{\012"
  "    currentfont /CharStrings get\012"
  "    currentfont /Encoding get\012"
  "    0 1 255 {\012"
  "      dup 8 mod 0 eq { () = } if\012"
  "      2 copy get 3 index exch known {\012"
  "        //metricsString exch 0 exch put\012"
  "        //metricsString //stringwidth exec\012"
  "        4 index idtransform		% to font's character coordinates\012"
  "        } {\012"
  "        pop 0 0\012"
  "        } ifelse\012"
  "      //showxy exec\012"
  "      } for\012"
  "      pop pop				% encoding, charstrings\012"
  "    } ifelse\012"
  "  pop				% fontmatrix\012"
  "  } bind def\012"
  "\012"
  "\012"
  "%\012"
  "% The main work: reportMetrics, reportEncoding, reportFont and report\012"
  "%\012"
  "\012"
  "/reportMetrics {		% stack: -> m\012"
  "				% privateDict is open\012"
  "  % Print new \042m\042 directive for currentfont\012"
  "  (QM ) print\012"
  "  lastMetrics 1 add /lastMetrics 1 index def\012"
  "  dup //printInt exec\012"
  "  //printMetrics exec\012"
  "  () =\012"
  "  } bind def\012"
  "\012"
  "/reportEncoding {		% stack: -> e\012"
  "				% privateDict is open\012"
  "  % Print new \042e\042 directive for currentfont\012"
  "  (QE ) print\012"
  "  lastEncoding 1 add /lastEncoding 1 index def\012"
  "  dup //printInt exec\012"
  "  //printMap exec\012"
  "  } bind def\012"
  "\012"
  "/spaceString (X) def		% for space char from current encoding\012"
  "\012"
  "/reportFont {			% stack: any -> unchanged\012"
  "  % Report currentfont to the user and record its number\012"
  "  % //privateDict is open\012"
  "  % Sets //privateDict/spaceString to font's encoding's space string, if any,\012"
  "  % and sets //privateDict/hasSpace to indicate whether there's a space char.\012"
  "  currentfont /UniqueID known not {\012"
  "    //reportMetrics exec\012"
  "    } {\012"
  "    //metrics currentfont /UniqueID get\012"
  "    2 copy known {\012"
  "      get\012"
  "      } {\012"
  "      //reportMetrics exec\012"
  "      dup 4 1 roll put		% define in /metrics\012"
  "      } ifelse\012"
  "    } ifelse			% stack: m\012"
  "  //encodings currentfont /Encoding get\012"
  "  2 copy known {\012"
  "    get\012"
  "    } {\012"
  "    //reportEncoding exec	% leaves e on stack\012"
  "    dup 4 1 roll put		% define in /encodings\012"
  "    % Find space character number for the encoding.\012"
  "    % Biased towards number 32, so avoid problems with weird encodings\012"
  "    //encodingSpace 1 index	% dict and key for putting the result\012"
  "    currentfont /Encoding get\012"
  "    dup length 32 le {\012"
  "      false\012"
  "      } {\012"
  "      dup 32 get dup /space eq exch /G20 eq or\012"
  "      } ifelse {\012"
  "      pop 32\012"
  "      } {\012"
  "      % It's not character number 32: search from 0\012"
  "      0 exch {\012"
  "        dup /space eq exch dup /G20 eq exch /suppress eq or or { exit } if\012"
  "        1 add\012"
  "        } forall\012"
  "      } ifelse\012"
  "    put				% put in /encodingSpace\012"
  "    } ifelse			% stack: m e\012"
  "  //encodingSpace 1 index get	% font's space character (256 if none)\012"
  "  /hasSpace 1 index 256 lt def\012"
  "  hasSpace { //spaceString exch 0 exch put } { pop } ifelse\012"
  "  (QF ) print\012"
  "  lastFontNum 1 add /lastFontNum 1 index def\012"
  "  //fonts currentfont 2 index put % record fontNum in /fonts\012"
  "  dup //printInt exec		% print fontNum\012"
  "				% stack: m e f\012"
  "  1000 0 //characterToReporting exec 2 copy //showxy exec\012"
  "  0 1000 //characterToReporting exec 2 copy //showxy exec\012"
  "  4 array astore		% stack: m e f array\012"
  "  //fontsUnit 3 1 roll put	% stack: m e\012"
  "  //printInt exec		% print encoding number\012"
  "  //printInt exec		% print metrics number\012"
  "  () =\012"
  "  } bind def\012"
  "\012"
  "/reportFontCreation {		% stack: font\012"
  "  % Report a newly created font. Called now so that if later uses are\012"
  "  % inside a save/restore we don't forget it.\012"
  "  % NOTE: this is currently not used, since it actually slows things down\012"
  "  //privateDict begin\012"
  "  inUse not {\012"
  "    /inUse true def\012"
  "    dup currentfont exch setfont //reportFont exec setfont\012"
  "    /inUse false def\012"
  "    } if\012"
  "  end\012"
  "  } bind def\012"
  "\012"
  "/reportSubString {		% stack: args string -> args\012"
  "  % Report the rendering of a string, assumed to be a single word.\012"
  "  % privateDict is open.  /n is font number, /p is call-back\012"
  "  dup length 0 eq {\012"
  "    p\012"
  "    } {\012"
  "    (QS ) print\012"
  "    n //printInt exec\012"
  "    //printCharacterOrigin exec\012"
  "    dup length //printInt exec\012"
  "    dup print ( ) print\012"
  "    /p load end			% close privateDict during the call-back\012"
  "    exec			% render the string; leaves args on stack\012"
  "    //privateDict begin\012"
  "    //printCharacterOrigin exec\012"
  "    () =\012"
  "    } ifelse\012"
  "  } bind def\012"
  "\012"
  "/report {			% stack: args string proc -> args\012"
  "  % Report the rendering of a string.\012"
  "  % Calls proc for each word and space.  The call-back should expect \042args\042\012"
  "  % on the stack followed by a string, and should leave \042args\042 on the stack.\012"
  "  //privateDict begin\012"
  "  inUse {\012"
  "    end exec\012"
  "    } {\012"
  "    /inUse true def\012"
  "    //fonts currentfont\012"
  "    2 copy known not { //reportFont exec } if\012"
  "    get				% stack: probable font-number\012"
  "    % check if points transform as before ...\012"
  "    //fontsUnit 1 index get\012"
  "    1000 0 //characterToReporting exec\012"
  "    0 1000 //characterToReporting exec\012"
  "    4 index 3 get ne 4 1 roll\012"
  "    4 index 2 get ne 4 1 roll\012"
  "    4 index 1 get ne 4 1 roll\012"
  "    4 index 0 get ne 5 -1 roll pop\012"
  "    or or or {			% if transformed points differ\012"
  "      //reportFont exec\012"
  "      pop //fonts currentfont get\012"
  "      } if			% stack: args string proc n\012"
  "    /n exch def\012"
  "    /p exch def		% stack: args string\012"
  "    hasSpace {\012"
  "      {			% begin loop\012"
  "        //spaceString search {\012"
  "          exch pop exch /s exch def\012"
  "          //reportSubString exec\012"
  "          //spaceString p\012"
  "          s\012"
  "          } {\012"
  "          //reportSubString exec\012"
  "          exit\012"
  "          } ifelse\012"
  "        } loop\012"
  "      } {\012"
  "      //reportSubString exec\012"
  "      } ifelse\012"
  "    /inUse false def\012"
  "    end\012"
  "    } ifelse\012"
  "  } bind def\012"
  "\012"
  "/dontReport {			% stack: proc\012"
  "  % Call \042proc\042 with //privateDict/inUse set to true\012"
  "  //privateDict /inUse get {\012"
  "    exec\012"
  "    } {\012"
  "    //privateDict /inUse true put\012"
  "    exec\012"
  "    //privateDict /inUse false put\012"
  "    } ifelse\012"
  "  } bind def\012"
  "\012"
  "/kshow.temp (X) def		% scratch space for kshow\012"
  "\012"
  "% Output from groff 1.08 sometimes uses ashow to show letters from \012"
  "% two separate words, with the offset being used to add the space.\012"
  "% Implement 'awidthshow' and friends as calls to 'show' to catch this.\012"
  "\012"
  "/pstotext_awidthshow { % cx cy char ax ay string --\012"
  "  //kshow.temp\012"
  "  0 1 3 index length 1 sub\012"
  "  {\012"
  "    2 index exch get\012"
  "    1 index 0 2 index put\012"
  "    1 index show\012"
  "    4 index 4 index rmoveto\012"
  "    5 index eq\012"
  "    { 6 index 6 index rmoveto } if\012"
  "  } for\012"
  "  pop pop pop pop pop pop pop\012"
  "} def\012"
  "\012"
  "/pstotext_xshow { % string numarray --\012"
  "  //kshow.temp\012"
  "  0 1 4 index length 1 sub\012"
  "  {\012"
  "    currentpoint 2 index\012"
  "    6 index exch get\012"
  "    4 index 0 2 index put pop\012"
  "    3 index show \012"
  "    moveto\012"
  "    % set char width from array\012"
  "    2 index exch get\012"
  "    0 rmoveto\012"
  "  } for\012"
  "  pop pop pop\012"
  "} def\012"
  "\012"
  "/pstotext_yshow { % string numarray --\012"
  "  //kshow.temp\012"
  "  0 1 4 index length 1 sub\012"
  "  {\012"
  "    currentpoint 2 index\012"
  "    6 index exch get\012"
  "    4 index 0 2 index put pop\012"
  "    3 index show\012"
  "    moveto\012"
  "    % set char height from array\012"
  "    2 index exch get\012"
  "    0 exch rmoveto\012"
  "  } for\012"
  "  pop pop pop\012"
  "} def\012"
  "\012"
  "/pstotext_xyshow { % string numarray --\012"
  "  //kshow.temp\012"
  "  0 1 4 index length 1 sub\012"
  "  {\012"
  "    currentpoint 2 index\012"
  "    6 index exch get\012"
  "    4 index 0 2 index put pop\012"
  "    3 index show\012"
  "    moveto\012"
  "    % set char width and height from array\012"
  "    dup add\012"
  "    2 index 1 index get exch\012"
  "    3 index exch 1 add get\012"
  "    rmoveto\012"
  "  } for\012"
  "  pop pop pop\012"
  "} def\012"
  "\012"
  "% Output inverse of initial currentmatrix, for possible use by postprocessor.\012"
  "(QI ) print\012"
  "matrix currentmatrix matrix invertmatrix\012"
  "{ 100 mul round cvi //printInt exec } forall\012"
  "() =\012"
  "\012"
  "userdict begin			% subsequent definitions are publicly visible\012"
  "\012"
  "% Objects placed in systemdict must be in global memory,\012"
  "% and must not reference local objects.\012"
  "/setglobal where\012"
  " { pop currentglobal true setglobal }\012"
  " { }\012"
  "ifelse\012"
  "\012"
  "\012"
  "%\012"
  "% Redefine the character rendering operations to call \042report\042\012"
  "%\012"
  "\012"
  "/show { { show }\012"
  "  //systemdict /pstotextLocalDict get /report get\012"
  "  exec } bind redef\012"
  "\012"
  "%/ashow { {3 copy ashow pop}\012"
  "%  //systemdict /pstotextLocalDict get /report get\012"
  "%  exec pop pop } bind redef\012"
  "\012"
  "% Implement as call to show to catch PostScript that uses\012"
  "% these to show letters from separate words.\012"
  "/ashow {\012"
  "   0 0 -1 6 3 roll\012"
  "  //systemdict /pstotextLocalDict get /pstotext_awidthshow get exec \012"
  "} bind redef\012"
  "\012"
  "/widthshow {\012"
  "   0 0 3 -1 roll\012"
  "  //systemdict /pstotextLocalDict get /pstotext_awidthshow get exec \012"
  "} bind redef\012"
  "\012"
  "/awidthshow {\012"
  "  //systemdict /pstotextLocalDict get /pstotext_awidthshow get exec \012"
  "} bind redef\012"
  "\012"
  "/xshow {\012"
  "  //systemdict /pstotextLocalDict get /pstotext_xshow get exec \012"
  "} bind redef\012"
  "\012"
  "/yshow {\012"
  "  //systemdict /pstotextLocalDict get /pstotext_yshow get exec \012"
  "} bind redef\012"
  "\012"
  "/xyshow {\012"
  "  //systemdict /pstotextLocalDict get /pstotext_xyshow get exec \012"
  "} bind redef\012"
  "\012"
  "/kshow {			% stack: proc string\012"
  "  exch //systemdict /pstotextLocalDict get exch /kshow.proc exch put\012"
  "  false exch			% stack: false string\012"
  "  {				% stack: false next | prev true next\012"
  "    //systemdict /pstotextLocaldict get /kshow.temp get 0 2 index put\012"
  "    exch { //systemdict /pstotextLocalDict get\012"
  "           /kshow.proc get exec\012"
  "         } { pop } ifelse\012"
  "    //systemdict /pstotextLocaldict get\012"
  "    /kshow.temp get //show exec\012"
  "    //systemdict /pstotextLocaldict get\012"
  "    /kshow.temp get 0 get true	% stack: this true\012"
  "    } forall\012"
  "				% stack: false | last true\012"
  "  { pop } if\012"
  "  } bind redef\012"
  "\012"
  "\012"
  "%\012"
  "% Redefine non-rendering operations so that they don't report\012"
  "%\012"
  "\012"
  "/stringwidth { {stringwidth}\012"
  "  //systemdict /pstotextLocalDict get /dontReport get\012"
  "  exec } bind redef\012"
  "\012"
  "/charpath { {charpath}\012"
  "  //systemdict /pstotextLocalDict get /dontReport get\012"
  "  exec } bind redef\012"
  "\012"
  "\012"
  "%\012"
  "% Intercept and report the page operations\012"
  "%\012"
  "\012"
  "/copypage { (QC) = flush copypage } bind redef\012"
  "\012"
  "/erasepage { (QZ) = flush erasepage } bind redef\012"
  "\012"
  "/showpage { (QP) = flush showpage } bind redef\012"
  "\012"
  "\012"
  "%\012"
  "% Intercept font creation so as to record the font inside less save/restore's\012"
  "% NOTE: disabled, because it actually slows things down\012"
  "%\012"
  "\012"
  "% /definefont { definefont //reportFontCreation exec } bind redef\012"
  "\012"
  "% /makefont { makefont //reportFontCreation exec } bind redef\012"
  "\012"
  "% /scalefont { scalefont //reportFontCreation exec } bind redef\012"
  "\012"
  "%\012"
  "% Clean-up\012"
  "%\012"
  "\012"
  "% restore local/global state\012"
  "/setglobal where\012"
  " { pop setglobal }\012"
  " { }\012"
  "ifelse\012"
  "\012"
  "end				% close nested userdict begin\012"
  "end				% close private dictionary\012"
  "\012"
  "% Bind the operators we just defined, and all the others if we didn't\012"
  "% do it before.  Also reenable 'bind' for future files.\012"
  "\012"
  "revision 353 ge {\012"
  ".bindoperators\012"
  "NOBIND currentdict systemdict ne and\012"
  " { systemdict begin .bindoperators end }\012"
  "if\012"
  "/DELAYBIND where { pop DELAYBIND { .bindnow } if } if\012"
  "} if\012"
  "\012"
  "systemdict readonly pop\012"
  "% Restore the current local/global VM mode.\012"
  "% exec\012"
  "\012"
  "\012"
  "%\012"
  "% Testing\012"
  "%\012"
  "\012"
  "false {\012"
  "  100 dict begin\012"
  "  (Times 12, two strings; second one sloping up with ashow:)=\012"
  "  /Times-Roman findfont 12 scalefont dup /t12 exch def setfont\012"
  "  72 300 moveto (Hello world) show\012"
  "  72 280 moveto 10 1 (Hello world once more) ashow\012"
  "  (Times 10 two strings:)=\012"
  "  /Times-Roman findfont 10 scalefont setfont\012"
  "  72 260 moveto (Third) show\012"
  "  72 240 moveto (Fourth) show\012"
  "  (Symbol 12, one string:)=\012"
  "  /Symbol findfont 12 scalefont setfont\012"
  "  72 220 moveto (symbol string) show\012"
  "  (Helvetica 12, two strings:)=\012"
  "  /Helvetica findfont 12 scalefont setfont\012"
  "  72 200 moveto (Fifth) show\012"
  "  72 180 moveto (Sixth) show\012"
  "  (Times 12 again, two strings; second one with kshow:)=\012"
  "  t12 setfont\012"
  "  72 160 moveto (Seventh) show\012"
  "  end\012"
  "  72 140 moveto\012"
  "  gsave\012"
  "    /dx 1.0 def\012"
  "    { pop pop\012"
  "      dx 1 add /dx 1 index def\012"
  "      0 rmoveto } (Accelerated letter spacing) kshow\012"
  "    grestore\012"
  "  (Times 12 scaled by 2:)=\012"
  "  72 100 moveto\012"
  "  gsave\012"
  "    2 2 scale\012"
  "    (Ninth) show\012"
  "    grestore\012"
  "  count 0 ne { (Left on stack:)= pstack } if\012"
  "  flush\012"
  "  } if\012"
  "\0\0"
END

