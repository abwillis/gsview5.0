?	 ?   ?i  % Copyright (C) 1995, Digital Equipment Corporation.
 % All rights reserved.
% See the file pstotext.txt for a full description.

% ocr.ps, part of BuildLectern
%
%   This is a PostScript library to send the characters rendered by a
%   PostScript job back on stdout.  The output is intended to allow
%   reconstruction of the document's words and an approximation of the words'
%   bounding rectangles.

% Last modified on Fri Jan  9 21:20:00 AEST 2004 by rjl
%      modified on Sat Feb  5 21:00:00 AEST 2000 by rjl
%      modified on Fri Oct  2 17:13:53 PDT 1998 by mcjones
%      modified on Thu Jan 25 15:24:37 PST 1996 by deutsch
%      modified on Wed May  3 15:41:30 PDT 1995 by birrell

% Modified by rjl
%   For Ghostscript 6 or later.
%   Change ashow, widthshow and awidthshow to call show for each character.
%   Add xshow, yshow, xyshow.

% Restrictions:
%
%   This library redefines some names that are originally defined as
%   operators, and of course the new definitions are procedures. Some
%   jobs might be sensitive to this distinction.  In this case, you'd
%   need to make "redef" create a new operator.  For example, "odef".
%
%   This library reports characters rendered by show, ashow, widthshow,
%   awidthshow, kshow, xshow, yshow and xyshow.  
%   The characters are reported even if they would
%   be invisible in final hardcopy.  This could happen, for example, if the
%   characters get clipped, or if they get overprinted, or if they are the
%   same color as their background, or if the font's glyph make no marks.
%
%   PostScript doesn't specify a standard character code for the characters
%   being rendered; rather, the font maps small integers into glyphs.  It's
%   not possible in general to determine a translation back into a standard
%   character code.  This library approximates this by giving mappings from
%   the job's characters to indexes into a table of known glyph names.  This
%   handles most PostScript jobs that we've encountered, but it's not an
%   absolute solution to the problem.

% Output format:
%
%    The "reporting coordinate system" is the device coordinate system.  The
%    application interpreting this library's output needs to understand the
%    device coordinate system.
%
%    Positions are reported in the reporting coordinate system, but with (x,y) 
%    values multiplied by 100 and rounded to integers.  Note that
%    positions might involve negative integers.
%
%    The directives, whitespace and numbers are themselves encoded in ASCII.
%    The strings in the "S" directives are in currentfont's encoding: they
%    should be treated as 8-bit binary data.
%
%    QI 6-integers
%                       Specifies the inverse of the currentmatrix for
%                       the current output device, after rotXXX.ps and
%                       before the document is processed.
%
%    QM m blx bly trx try 256-pairs
%			Introduces a new character metrics table used by
%			some font, where "m" is an integer that identifies
%			the metrics table in subsequent "F" directives.  "m"
%			is small.  (blx,bly) is the bottom left corner of the
%			font's bounding box, and (trx,try) is its top right. 
%			The pairs are the stringwidth of each character in the
%			font's encoding.  All of these are reported in the
%			font's character coordinate system, multiplied by 100
%			and rounded to integers.  The metrics
%			table will be referenced in a subsequent "F"
%			directive, which includes information mapping the
%			font's metrics to the initial user coordinate system. 
%			Note that values of "m" might get reused, after a
%			"restore".
%
% QE e n n-integers	Introduces a new encoding, where "e" is an integer
%			that identifies the encoding in subsequent "F"
%			directives.  "e" is small.  "e" is followed by
%			an integer "n" and then by a sequence of
%			exactly n integers.  The i'th integer
%			specifies the glyph for the i'th entry of the
%			font's encoding vector.  The value of the
%			integer is usually an index in the array
%			"StandardGlyphs", defined below.  Note that
%			the first 256 entries of that array equal the
%			corresponding entries of ISOLatin1Encoding.
%			The value 9999 indicates that this entry in
%			the font's encoding specifies a glyph not
%			named in StandardGlyphs.  Note that values of
%			"e" might get reused, after a "restore".
%
%    QF n x y x' y' e m	Introduces a new font, where "n" is an integer
%			that identifies the font in subsequent "S"
%			directives.  "n" is small.  (x,y) is the
%			position corresponding to (1000,0) in the font's
%			character coordinate system, and (x',y') is the
%			position corresponding to (0,1000), both as they would
%			be if the character were drawn with its origin at
%			the origin of the reporting coordinate system. "e" is
%			an integer specifying a previously defined
%			encoding vector.  "m" is an integer specifying a
%			previously defined metrics table.  Note that values
%			of "n" might get reused, after a "restore".
%
%    QS n x y l s x' y'	Reports rendering of string "s" in font "n".
%			(x,y) is the position corresponding to the origin of
%			the first character. "l" is length of the string,
%			followed immediately by a single space then the string.
%			(x',y') is the position that would correspond to the
%			origin of a subsequent character. The
%			string reported by this directive is never empty.
%			The string also never contains a "space": strings
%			that would have contained a "space" are split up
%			into multiple directives, with the "space"
%			omitted.  Here "space" means the first character
%			in the font's encoding that maps to the glyph
%			named "/space", if there is such a character.
%
%    QC			copypage was invoked
%
%    QZ			erasepage was invoked
%
%    QP			showpage was invoked

%
% globals and subroutines
%

%/setglobal where
% { pop currentglobal /setglobal load true setglobal }
% { { } }
%ifelse
revision 353 ge {
NOBIND /DELAYBIND where { pop DELAYBIND or } if
 { systemdict begin
                /bind /.bind load
                  /.forcedef where { pop .forcedef } { def } ifelse
              end
 }
if
} if

% put our private stuff in a local dictionary,
% but place a reference to it in systemdict
systemdict begin
/pstotextLocalDict 30 dict /.forcedef where { pop .forcedef } { def } ifelse
end

pstotextLocalDict begin		% following stuff is private
/redef { systemdict begin 1 index exch .makeoperator def end } bind def
/privateDict currentdict def	% for lastFontNum and lastEncoding
/fonts 200 dict def		% maps font to integer "n"
/fontsUnit 200 dict def 	% (1000,0) and (0,1000) transformed, per font
/encodings 200 dict def		% maps encoding array to integer "e"
/encodingSpace 200 dict def	% space char for each encoding
/metrics 200 dict def		% maps font UniqueID to integer  "m"
/lastFontNum 0 def		% last integer used for a font
/lastEncoding 0 def		% last integer used for an encoding
/lastMetrics 0 def		% last integer used for a metrics table
/tempString 20 string def	% scratch for printing integers
/reportMatrix matrix identmatrix def % maps device coords to reporting coords
/inUse false def		% prevents recursive invokcation of "report"

/TimesRomanGlyphs [
  % ISOLatin1Encoding ...
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /space /exclam /quotedbl
  /numbersign /dollar /percent /ampersand /quoteright
  /parenleft /parenright /asterisk /plus /comma
  /minus /period /slash /zero /one
  % 50
  /two /three /four /five /six
  /seven /eight /nine /colon /semicolon
  /less /equal /greater /question /at
  /A /B /C /D /E
  /F /G /H /I /J
  /K /L /M /N /O
  /P /Q /R /S /T
  /U /V /W /X /Y
  /Z /bracketleft /backslash /bracketright /asciicircum
  /underscore /quoteleft /a /b /c
  % 100
  /d /e /f /g /h
  /i /j /k /l /m
  /n /o /p /q /r
  /s /t /u /v /w
  /x /y /z /braceleft /bar
  /braceright /asciitilde /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /.notdef
  /.notdef /.notdef /.notdef /.notdef /dotlessi
  /grave /acute /circumflex /tilde /macron
  % 150
  /breve /dotaccent /dieresis /.notdef /ring
  /cedilla /.notdef /hungarumlaut /ogonek /caron
  /space /exclamdown /cent /sterling /currency
  /yen /brokenbar /section /dieresis /copyright
  /ordfeminine /guillemotleft /logicalnot /hyphen /registered
  /macron /degree /plusminus /twosuperior /threesuperior
  /acute /mu /paragraph /periodcentered /cedilla
  /onesuperior /ordmasculine /guillemotright /onequarter /onehalf
  /threequarters /questiondown /Agrave /Aacute /Acircumflex
  /Atilde /Adieresis /Aring /AE /Ccedilla
  % 200
  /Egrave /Eacute /Ecircumflex /Edieresis /Igrave
  /Iacute /Icircumflex /Idieresis /Eth /Ntilde
  /Ograve /Oacute /Ocircumflex /Otilde /Odieresis
  /multiply /Oslash /Ugrave /Uacute /Ucircumflex
  /Udieresis /Yacute /Thorn /germandbls /agrave
  /aacute /acircumflex /atilde /adieresis /aring
  /ae /ccedilla /egrave /eacute /ecircumflex
  /edieresis /igrave /iacute /icircumflex /idieresis
  /eth /ntilde /ograve /oacute /ocircumflex
  /otilde /odieresis /divide /oslash /ugrave
  % 250
  /uacute /ucircumflex /udieresis /yacute /thorn
  /ydieresis 
  % Other glyphs from /Times-Roman ...
  % 256
  /quotedblright /Scaron /dagger /guilsinglleft
  /Zcaron /daggerdbl /Lslash /ellipsis /guilsinglright
  /oe /fi /bullet /perthousand /quotedblbase
  /endash /emdash /trademark /florin /lslash
  /scaron /Ydieresis /fl /fraction /quotedblleft
  /quotesinglbase /quotesingle /zcaron /OE
  % 284
  ] def

/dvipsGlyphs [
  % Self-named glyphs for dvitops type 3 fonts ...
  % 284
  0 1 255 { 1 string dup 0 4 -1 roll put cvn } for
  % 540
  ] def

/ttypeGlyphs1 [
  % Glyph names used by MS TrueType encodings ...
  % 540
  /G00 /G01 /G02 /G03 /G04 /G05 /G06 /G07 /G08 /G09 /G0a /G0b /G0c /G0d /G0e
  /G0f /G10 /G11 /G12 /G13 /G14 /G15 /G16 /G17 /G18 /G19 /G1a /G1b /G1c /G1d
  /G1e /G1f /G20 /G21 /G22 /G23 /G24 /G25 /G26 /G27 /G28 /G29 /G2a /G2b /G2c
  /G2d /G2e /G2f /G30 /G31 /G32 /G33 /G34 /G35 /G36 /G37 /G38 /G39 /G3a /G3b
  % 600
  /G3c /G3d /G3e /G3f /G40 /G41 /G42 /G43 /G44 /G45 /G46 /G47 /G48 /G49 /G4a
  /G4b /G4c /G4d /G4e /G4f /G50 /G51 /G52 /G53 /G54 /G55 /G56 /G57 /G58 /G59
  /G5a /G5b /G5c /G5d /G5e /G5f /G60 /G61 /G62 /G63 /G64 /G65 /G66 /G67 /G68
  /G69 /G6a /G6b /G6c /G6d /G6e /G6f /G70 /G71 /G72 /G73 /G74 /G75 /G76 /G77
  /G78 /G79 /G7a /G7b /G7c /G7d /G7e /G7f /G80 /G81 /G82 /G83 /G84 /G85 /G86
  /G87 /G88 /G89 /G8a /G8b /G8c /G8d /G8e /G8f /G90 /G91 /G92 /G93 /G94 /G95
  /G96 /G97 /G98 /G99 /G9a /G9b /G9c /G9d /G9e /G9f /Ga0 /Ga1 /Ga2 /Ga3 /Ga4
  /Ga5 /Ga6 /Ga7 /Ga8 /Ga9 /Gaa /Gab /Gac /Gad /Gae /Gaf /Gb0 /Gb1 /Gb2 /Gb3
  /Gb4 /Gb5 /Gb6 /Gb7 /Gb8 /Gb9 /Gba /Gbb /Gbc /Gbd /Gbe /Gbf /Gc0 /Gc1 /Gc2
  /Gc3 /Gc4 /Gc5 /Gc6 /Gc7 /Gc8 /Gc9 /Gca /Gcb /Gcc /Gcd /Gce /Gcf /Gd0 /Gd1
  % 750
  /Gd2 /Gd3 /Gd4 /Gd5 /Gd6 /Gd7 /Gd8 /Gd9 /Gda /Gdb /Gdc /Gdd /Gde /Gdf /Ge0
  /Ge1 /Ge2 /Ge3 /Ge4 /Ge5 /Ge6 /Ge7 /Ge8 /Ge9 /Gea /Geb /Gec /Ged /Gee /Gef
  /Gf0 /Gf1 /Gf2 /Gf3 /Gf4 /Gf5 /Gf6 /Gf7 /Gf8 /Gf9 /Gfa /Gfb /Gfc /Gfd /Gfe
  /Gff 
  % 796
  ] def

/ttypeGlyphs2 [
  % 796
  /G00 /G01 /G02 /G03 /G04 /G05 /G06 /G07 /G08 /G09 /G0A /G0B /G0C /G0D
  /G0E /G0F /G10 /G11 /G12 /G13 /G14 /G15 /G16 /G17 /G18 /G19 /G1A /G1B /G1C
  /G1D /G1E /G1F /G20 /G21 /G22 /G23 /G24 /G25 /G26 /G27 /G28 /G29 /G2A /G2B
  /G2C /G2D /G2E /G2F /G30 /G31 /G32 /G33 /G34 /G35 /G36 /G37 /G38 /G39 /G3A
  /G3B /G3C /G3D /G3E /G3F /G40 /G41 /G42 /G43 /G44 /G45 /G46 /G47 /G48 /G49
  /G4A /G4B /G4C /G4D /G4E /G4F /G50 /G51 /G52 /G53 /G54 /G55 /G56 /G57 /G58
  /G59 /G5A /G5B /G5C /G5D /G5E /G5F /G60 /G61 /G62 /G63 /G64 /G65 /G66 /G67
  % 900
  /G68 /G69 /G6A /G6B /G6C /G6D /G6E /G6F /G70 /G71 /G72 /G73 /G74 /G75 /G76
  /G77 /G78 /G79 /G7A /G7B /G7C /G7D /G7E /G7F /G80 /G81 /G82 /G83 /G84 /G85
  /G86 /G87 /G88 /G89 /G8A /G8B /G8C /G8D /G8E /G8F /G90 /G91 /G92 /G93 /G94
  /G95 /G96 /G97 /G98 /G99 /G9A /G9B /G9C /G9D /G9E /G9F /GA0 /GA1 /GA2 /GA3
  /GA4 /GA5 /GA6 /GA7 /GA8 /GA9 /GAA /GAB /GAC /GAD /GAE /GAF /GB0 /GB1 /GB2
  /GB3 /GB4 /GB5 /GB6 /GB7 /GB8 /GB9 /GBA /GBB /GBC /GBD /GBE /GBF /GC0 /GC1
  /GC2 /GC3 /GC4 /GC5 /GC6 /GC7 /GC8 /GC9 /GCA /GCB /GCC /GCD /GCE /GCF /GD0
  /GD1 /GD2 /GD3 /GD4 /GD5 /GD6 /GD7 /GD8 /GD9 /GDA /GDB /GDC /GDD /GDE /GDF
  /GE0 /GE1 /GE2 /GE3 /GE4 /GE5 /GE6 /GE7 /GE8 /GE9 /GEA /GEB /GEC /GED /GEE
  /GEF /GF0 /GF1 /GF2 /GF3 /GF4 /GF5 /GF6 /GF7 /GF8 /GF9 /GFA /GFB /GFC /GFD
  % 1050
  /GFE /GFF 
  % 1052
  ] def

/oldDviGlyphs [
  % More self-named glyphs for old dvitops type 3 fonts ...
  % 1052
  0 1 127 { 10 3 string cvrs cvn } for
  % 1180
  ] def

/StandardGlyphs //TimesRomanGlyphs length 896 add array def
//StandardGlyphs 0 //TimesRomanGlyphs putinterval
//StandardGlyphs //TimesRomanGlyphs length //dvipsGlyphs putinterval
//StandardGlyphs //TimesRomanGlyphs length 256 add //ttypeGlyphs1 putinterval
//StandardGlyphs //TimesRomanGlyphs length 512 add //ttypeGlyphs2 putinterval
//StandardGlyphs //TimesRomanGlyphs length 768 add //oldDviGlyphs putinterval

/standardMap StandardGlyphs length dict
  % Maps names to indices in StandardGlyphs.
  0 StandardGlyphs {
    2 index exch
    2 copy known { pop pop } { 2 index put } ifelse
    1 add
    } forall
  pop def

/printInt {			% stack: n
  % Prints an integer followed by a space on stdout
  //tempString cvs print ( ) print
  } bind def

/showxy {			%stack:  x y
  % prints a pair of integers on stdout, converting to 1/100th's
  exch
  100 mul round cvi //printInt exec
  100 mul round cvi //printInt exec
  } bind def

/characterToReporting {		% stack: x y -> x' y'
  % Transforms a vector in currentfont's character coordinate
  % system to the reporting coordinate system
  currentfont /FontMatrix get
  dtransform			% to current user coordinates
  dtransform			% to device coordinates
  //reportMatrix idtransform	% to reporting coordinates
  } def

/printCharacterOrigin {
  % Prints the position in the reporting coordinate system at which a
  % character origin would be painted using currentfont
  0 0 currentfont /FontMatrix get transform
  currentpoint exch 4 -1 roll add 3 1 roll add
  transform //reportMatrix itransform
  //showxy exec
  } bind def

/printMap {
  % Print map from indices in currentfont/Encoding to StandardEncoding indices
  16 currentfont /Encoding get dup length //printInt exec {
    exch dup 16 eq { () = pop 1 } { 1 add } ifelse exch
    //standardMap exch
    2 copy known {
      get //printInt exec
      } {
      pop pop (9999 ) print
      } ifelse
    } forall
  pop () =
  } bind def

/metricsString (X) def

/printMetrics {
  % Print bounding box and character metrics for currentfont
  % Sadly, dvitops produces illegal type 3 fonts with no /.notdef entry. The
  % use of "stopped" deals with that and any other silliness.
  currentfont /FontBBox get aload pop 4 2 roll
  //showxy exec
  //showxy exec
  currentfont /FontMatrix get
  currentfont /FontType get 3 eq {
    //privateDict /pm.dictCount countdictstack put
    count //privateDict exch /pm.count exch put
    //privateDict /pm.save save put
    0 1 255 {
      dup 8 mod 0 eq { () = } if
      //metricsString exch 0 exch put
      { //metricsString //stringwidth exec
        2 index idtransform		% to font's character coordinates
        } stopped {
        count //privateDict /pm.count get sub { pop } repeat
        countdictstack //privateDict /pm.dictCount get sub { end } repeat
        //privateDict /pm.save get restore
        //privateDict /pm.save save put
        0 0
        } if
      //showxy exec
      } for
    //privateDict /pm.save get restore % necessary to balance the "save"
    }{
    currentfont /CharStrings get
    currentfont /Encoding get
    0 1 255 {
      dup 8 mod 0 eq { () = } if
      2 copy get 3 index exch known {
        //metricsString exch 0 exch put
        //metricsString //stringwidth exec
        4 index idtransform		% to font's character coordinates
        } {
        pop 0 0
        } ifelse
      //showxy exec
      } for
      pop pop				% encoding, charstrings
    } ifelse
  pop				% fontmatrix
  } bind def


%
% The main work: reportMetrics, reportEncoding, reportFont and report
%

/reportMetrics {		% stack: -> m
				% privateDict is open
  % Print new "m" directive for currentfont
  (QM ) print
  lastMetrics 1 add /lastMetrics 1 index def
  dup //printInt exec
  //printMetrics exec
  () =
  } bind def

/reportEncoding {		% stack: -> e
				% privateDict is open
  % Print new "e" directive for currentfont
  (QE ) print
  lastEncoding 1 add /lastEncoding 1 index def
  dup //printInt exec
  //printMap exec
  } bind def

/spaceString (X) def		% for space char from current encoding

/reportFont {			% stack: any -> unchanged
  % Report currentfont to the user and record its number
  % //privateDict is open
  % Sets //privateDict/spaceString to font's encoding's space string, if any,
  % and sets //privateDict/hasSpace to indicate whether there's a space char.
  currentfont /UniqueID known not {
    //reportMetrics exec
    } {
    //metrics currentfont /UniqueID get
    2 copy known {
      get
      } {
      //reportMetrics exec
      dup 4 1 roll put		% define in /metrics
      } ifelse
    } ifelse			% stack: m
  //encodings currentfont /Encoding get
  2 copy known {
    get
    } {
    //reportEncoding exec	% leaves e on stack
    dup 4 1 roll put		% define in /encodings
    % Find space character number for the encoding.
    % Biased towards number 32, so avoid problems with weird encodings
    //encodingSpace 1 index	% dict and key for putting the result
    currentfont /Encoding get
    dup length 32 le {
      false
      } {
      dup 32 get dup /space eq exch /G20 eq or
      } ifelse {
      pop 32
      } {
      % It's not character number 32: search from 0
      0 exch {
        dup /space eq exch dup /G20 eq exch /suppress eq or or { exit } if
        1 add
        } forall
      } ifelse
    put				% put in /encodingSpace
    } ifelse			% stack: m e
  //encodingSpace 1 index get	% font's space character (256 if none)
  /hasSpace 1 index 256 lt def
  hasSpace { //spaceString exch 0 exch put } { pop } ifelse
  (QF ) print
  lastFontNum 1 add /lastFontNum 1 index def
  //fonts currentfont 2 index put % record fontNum in /fonts
  dup //printInt exec		% print fontNum
				% stack: m e f
  1000 0 //characterToReporting exec 2 copy //showxy exec
  0 1000 //characterToReporting exec 2 copy //showxy exec
  4 array astore		% stack: m e f array
  //fontsUnit 3 1 roll put	% stack: m e
  //printInt exec		% print encoding number
  //printInt exec		% print metrics number
  () =
  } bind def

/reportFontCreation {		% stack: font
  % Report a newly created font. Called now so that if later uses are
  % inside a save/restore we don't forget it.
  % NOTE: this is currently not used, since it actually slows things down
  //privateDict begin
  inUse not {
    /inUse true def
    dup currentfont exch setfont //reportFont exec setfont
    /inUse false def
    } if
  end
  } bind def

/reportSubString {		% stack: args string -> args
  % Report the rendering of a string, assumed to be a single word.
  % privateDict is open.  /n is font number, /p is call-back
  dup length 0 eq {
    p
    } {
    (QS ) print
    n //printInt exec
    //printCharacterOrigin exec
    dup length //printInt exec
    dup print ( ) print
    /p load end			% close privateDict during the call-back
    exec			% render the string; leaves args on stack
    //privateDict begin
    //printCharacterOrigin exec
    () =
    } ifelse
  } bind def

/report {			% stack: args string proc -> args
  % Report the rendering of a string.
  % Calls proc for each word and space.  The call-back should expect "args"
  % on the stack followed by a string, and should leave "args" on the stack.
  //privateDict begin
  inUse {
    end exec
    } {
    /inUse true def
    //fonts currentfont
    2 copy known not { //reportFont exec } if
    get				% stack: probable font-number
    % check if points transform as before ...
    //fontsUnit 1 index get
    1000 0 //characterToReporting exec
    0 1000 //characterToReporting exec
    4 index 3 get ne 4 1 roll
    4 index 2 get ne 4 1 roll
    4 index 1 get ne 4 1 roll
    4 index 0 get ne 5 -1 roll pop
    or or or {			% if transformed points differ
      //reportFont exec
      pop //fonts currentfont get
      } if			% stack: args string proc n
    /n exch def
    /p exch def		% stack: args string
    hasSpace {
      {			% begin loop
        //spaceString search {
          exch pop exch /s exch def
          //reportSubString exec
          //spaceString p
          s
          } {
          //reportSubString exec
          exit
          } ifelse
        } loop
      } {
      //reportSubString exec
      } ifelse
    /inUse false def
    end
    } ifelse
  } bind def

/dontReport {			% stack: proc
  % Call "proc" with //privateDict/inUse set to true
  //privateDict /inUse get {
    exec
    } {
    //privateDict /inUse true put
    exec
    //privateDict /inUse false put
    } ifelse
  } bind def

/kshow.temp (X) def		% scratch space for kshow

% Output from groff 1.08 sometimes uses ashow to show letters from 
% two separate words, with the offset being used to add the space.
% Implement 'awidthshow' and friends as calls to 'show' to catch this.

/pstotext_awidthshow { % cx cy char ax ay string --
  //kshow.temp
  0 1 3 index length 1 sub
  {
    2 index exch get
    1 index 0 2 index put
    1 index show
    4 index 4 index rmoveto
    5 index eq
    { 6 index 6 index rmoveto } if
  } for
  pop pop pop pop pop pop pop
} def

/pstotext_xshow { % string numarray --
  //kshow.temp
  0 1 4 index length 1 sub
  {
    currentpoint 2 index
    6 index exch get
    4 index 0 2 index put pop
    3 index show 
    moveto
    % set char width from array
    2 index exch get
    0 rmoveto
  } for
  pop pop pop
} def

/pstotext_yshow { % string numarray --
  //kshow.temp
  0 1 4 index length 1 sub
  {
    currentpoint 2 index
    6 index exch get
    4 index 0 2 index put pop
    3 index show
    moveto
    % set char height from array
    2 index exch get
    0 exch rmoveto
  } for
  pop pop pop
} def

/pstotext_xyshow { % string numarray --
  //kshow.temp
  0 1 4 index length 1 sub
  {
    currentpoint 2 index
    6 index exch get
    4 index 0 2 index put pop
    3 index show
    moveto
    % set char width and height from array
    dup add
    2 index 1 index get exch
    3 index exch 1 add get
    rmoveto
  } for
  pop pop pop
} def

% Output inverse of initial currentmatrix, for possible use by postprocessor.
(QI ) print
matrix currentmatrix matrix invertmatrix
{ 100 mul round cvi //printInt exec } forall
() =

userdict begin			% subsequent definitions are publicly visible

% Objects placed in systemdict must be in global memory,
% and must not reference local objects.
/setglobal where
 { pop currentglobal true setglobal }
 { }
ifelse


%
% Redefine the character rendering operations to call "report"
%

/show { { show }
  //systemdict /pstotextLocalDict get /report get
  exec } bind redef

%/ashow { {3 copy ashow pop}
%  //systemdict /pstotextLocalDict get /report get
%  exec pop pop } bind redef

% Implement as call to show to catch PostScript that uses
% these to show letters from separate words.
/ashow {
   0 0 -1 6 3 roll
  //systemdict /pstotextLocalDict get /pstotext_awidthshow get exec 
} bind redef

/widthshow {
   0 0 3 -1 roll
  //systemdict /pstotextLocalDict get /pstotext_awidthshow get exec 
} bind redef

/awidthshow {
  //systemdict /pstotextLocalDict get /pstotext_awidthshow get exec 
} bind redef

/xshow {
  //systemdict /pstotextLocalDict get /pstotext_xshow get exec 
} bind redef

/yshow {
  //systemdict /pstotextLocalDict get /pstotext_yshow get exec 
} bind redef

/xyshow {
  //systemdict /pstotextLocalDict get /pstotext_xyshow get exec 
} bind redef

/kshow {			% stack: proc string
  exch //systemdict /pstotextLocalDict get exch /kshow.proc exch put
  false exch			% stack: false string
  {				% stack: false next | prev true next
    //systemdict /pstotextLocaldict get /kshow.temp get 0 2 index put
    exch { //systemdict /pstotextLocalDict get
           /kshow.proc get exec
         } { pop } ifelse
    //systemdict /pstotextLocaldict get
    /kshow.temp get //show exec
    //systemdict /pstotextLocaldict get
    /kshow.temp get 0 get true	% stack: this true
    } forall
				% stack: false | last true
  { pop } if
  } bind redef


%
% Redefine non-rendering operations so that they don't report
%

/stringwidth { {stringwidth}
  //systemdict /pstotextLocalDict get /dontReport get
  exec } bind redef

/charpath { {charpath}
  //systemdict /pstotextLocalDict get /dontReport get
  exec } bind redef


%
% Intercept and report the page operations
%

/copypage { (QC) = flush copypage } bind redef

/erasepage { (QZ) = flush erasepage } bind redef

/showpage { (QP) = flush showpage } bind redef


%
% Intercept font creation so as to record the font inside less save/restore's
% NOTE: disabled, because it actually slows things down
%

% /definefont { definefont //reportFontCreation exec } bind redef

% /makefont { makefont //reportFontCreation exec } bind redef

% /scalefont { scalefont //reportFontCreation exec } bind redef

%
% Clean-up
%

% restore local/global state
/setglobal where
 { pop setglobal }
 { }
ifelse

end				% close nested userdict begin
end				% close private dictionary

% Bind the operators we just defined, and all the others if we didn't
% do it before.  Also reenable 'bind' for future files.

revision 353 ge {
.bindoperators
NOBIND currentdict systemdict ne and
 { systemdict begin .bindoperators end }
if
/DELAYBIND where { pop DELAYBIND { .bindnow } if } if
} if

systemdict readonly pop
% Restore the current local/global VM mode.
% exec


%
% Testing
%

false {
  100 dict begin
  (Times 12, two strings; second one sloping up with ashow:)=
  /Times-Roman findfont 12 scalefont dup /t12 exch def setfont
  72 300 moveto (Hello world) show
  72 280 moveto 10 1 (Hello world once more) ashow
  (Times 10 two strings:)=
  /Times-Roman findfont 10 scalefont setfont
  72 260 moveto (Third) show
  72 240 moveto (Fourth) show
  (Symbol 12, one string:)=
  /Symbol findfont 12 scalefont setfont
  72 220 moveto (symbol string) show
  (Helvetica 12, two strings:)=
  /Helvetica findfont 12 scalefont setfont
  72 200 moveto (Fifth) show
  72 180 moveto (Sixth) show
  (Times 12 again, two strings; second one with kshow:)=
  t12 setfont
  72 160 moveto (Seventh) show
  end
  72 140 moveto
  gsave
    /dx 1.0 def
    { pop pop
      dx 1 add /dx 1 index def
      0 rmoveto } (Accelerated letter spacing) kshow
    grestore
  (Times 12 scaled by 2:)=
  72 100 moveto
  gsave
    2 2 scale
    (Ninth) show
    grestore
  count 0 ne { (Left on stack:)= pstack } if
  flush
  } if
 ?	 ?   ?
  % Copyright (C) 1995, Digital Equipment Corporation.
 % All rights reserved.
% See the file pstotext.txt for a full description.
%
% Lectern:
%   PostScript to be prepended to a job to rotate its images by //angle,
%   adjusting the page shape appropriately.  This works only for
%   devices (such as ppmraw) that allow arbitrary imaging areas.

% Last modified on Thu Aug  1 11:36:13 PDT 1996 by mcjones
%      modified on Tue Mar 14 14:54:44 PST 1995 by birrell

4 dict begin
/myDict currentdict def
/angle 270 def

/languagelevel where { pop languagelevel 2 ge } { FALSE } ifelse 
  {
  % Use setpagedevice for level 2 implementations
  /lastW 0 def
  /lastH 0 def
  <<
    /BeginPage {
      //myDict begin
      pop % page number
      currentpagedevice /PageSize get aload pop % stack: W H
%      (H = ) print dup =
%      (W = ) print 1 index = flush
%      (lastH = ) print lastH =
%      (lastW = ) print lastW = flush
      lastH ne exch lastW ne or {
        % /PageSize has changed: rotate it
        <<
          /PageSize [
            currentpagedevice /PageSize get aload pop % stack: W H
            exch % stack: newW newH
            /lastW 2 index def
            /lastH 1 index def
            ]
          >> setpagedevice % calls BeginPage recursively
%        (H := ) print lastH =
%        (W := ) print lastW = flush
        } {
        % Rotate ctm.  Don't do this if we're also adjusting PageSize,
        % because that causes another BeginPage call, which does this.
        currentpagedevice /PageSize get aload pop % stack: width height
        2 div exch 2 div exch % stack: center
        2 copy translate //angle rotate neg exch neg translate
        } ifelse
      end % //myDict
      }
    >> setpagedevice
  } {
  % Use ghostscript specific code for earlier systems
  % flip current frame buffer
    currentdevice getdeviceprops >> /HWSize get
    aload pop exch 2 array astore
    mark exch /HWSize exch currentdevice putdeviceprops pop
  % fix showpage to add a rotation and translation
    /adbFixup {
        currentdevice getdeviceprops >>
        dup /HWSize get aload pop 3 -1 roll
        /HWResolution get aload pop
        % stack: xsize ysize xres yres
        exch 4 -1 roll exch div 72 mul
        % stack: ysize yres xpts
        3 1 roll div 72 mul
        % stack: xpts ypts
        2 div exch 2 div exch
        2 copy translate //angle rotate neg exch neg translate
        } bind odef
    /adbShowpage /showpage load def
    userdict begin /showpage { adbShowpage adbFixup } bind odef end
    adbFixup
  % define new ".setpagesize" that flips its arguments
    statusdict begin
        /adbSet /.setpagesize load def
        /.setpagesize {
          exch adbSet adbFixup
          } bind def
        end
  } ifelse

end % //myDict
 ?	 ?   ?
  % Copyright (C) 1995, Digital Equipment Corporation.
 % All rights reserved.
% See the file pstotext.txt for a full description.
%
% Lectern:
%   PostScript to be prepended to a job to rotate its images by //angle,
%   adjusting the page shape appropriately.  This works only for
%   devices (such as ppmraw) that allow arbitrary imaging areas.

% Last modified on Thu Aug  1 11:36:25 PDT 1996 by mcjones
%      modified on Tue Mar 14 14:55:13 PST 1995 by birrell

4 dict begin
/myDict currentdict def
/angle 90 def

/languagelevel where { pop languagelevel 2 ge } { FALSE } ifelse 
  {
  % Use setpagedevice for level 2 implementations
  /lastW 0 def
  /lastH 0 def
  <<
    /BeginPage {
      //myDict begin
      pop % page number
      currentpagedevice /PageSize get aload pop % stack: W H
%      (H = ) print dup =
%      (W = ) print 1 index = flush
%      (lastH = ) print lastH =
%      (lastW = ) print lastW = flush
      lastH ne exch lastW ne or {
        % /PageSize has changed: rotate it
        <<
          /PageSize [
            currentpagedevice /PageSize get aload pop % stack: W H
            exch % stack: newW newH
            /lastW 2 index def
            /lastH 1 index def
            ]
          >> setpagedevice % calls BeginPage recursively
%        (H := ) print lastH =
%        (W := ) print lastW = flush
        } {
        % Rotate ctm.  Don't do this if we're also adjusting PageSize,
        % because that causes another BeginPage call, which does this.
        currentpagedevice /PageSize get aload pop % stack: width height
        2 div exch 2 div exch % stack: center
        2 copy translate //angle rotate neg exch neg translate
        } ifelse
      end % //myDict
      }
    >> setpagedevice
  } {
  % Use ghostscript specific code for earlier systems
  % flip current frame buffer
    currentdevice getdeviceprops >> /HWSize get
    aload pop exch 2 array astore
    mark exch /HWSize exch currentdevice putdeviceprops pop
  % fix showpage to add a rotation and translation
    /adbFixup {
        currentdevice getdeviceprops >>
        dup /HWSize get aload pop 3 -1 roll
        /HWResolution get aload pop
        % stack: xsize ysize xres yres
        exch 4 -1 roll exch div 72 mul
        % stack: ysize yres xpts
        3 1 roll div 72 mul
        % stack: xpts ypts
        2 div exch 2 div exch
        2 copy translate //angle rotate neg exch neg translate
        } bind odef
    /adbShowpage /showpage load def
    userdict begin /showpage { adbShowpage adbFixup } bind odef end
    adbFixup
  % define new ".setpagesize" that flips its arguments
    statusdict begin
        /adbSet /.setpagesize load def
        /.setpagesize {
          exch adbSet adbFixup
          } bind def
        end
  } ifelse

end % //myDict
 