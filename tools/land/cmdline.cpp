52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
Examples:

	land -tabsize[=]# [-r[#][,#]] [-copyright=<string>] <filename...>

	chargrab (-r -f -0 {-2 -7 -8 -16} -c[=]# -n -pal[=]# -P -o -d 
		-h -s -w -v -b (inputLbm[,outputChr][,outputMap]...)...)

		chargrab -options -n *.lbm,test.chr[,*.map] -n- test2.lbm
		chargrab -options -n test.lbm test1.lbm


	chargrab (-r -f -0 {-2 -7 -8 -16} -c[=]# -n -pal[=]# -P -o -d 
		-h -s -w -v -b
		inputLbm[.lbm]
		[-oc outputChr[.chr]]
		[-om outputMap[.map]]...)...)


	chargrab -n test1.lbm -n- test2.lbm -oc test1.chr

"	chargrab -n %f.lbm -oc %f.chr -om %f.map !	"


	Elements required for command line smarts:

	;------------------------------------------------------------------------
	Switch name/template:	"pal[=]#"
	Description:				Force palette offset
	Flags:						Number
									Range, 0--7 (SNES), 0--3 (Genesis)
	Default:						0

	;------------------------------------------------------------------------
	Switch name/template:	"flip"
	Description:				Character flip checking
	Flags:						Switch ( changes name to name[+|-]?? )
	Default:						TRUE

	;------------------------------------------------------------------------
	Switch name/template:	%f.lbm,%f.map,%f.chr
	Description:
	Flags:						Filename(s)
									Required
	Default:						(none)

	;------------------------------------------------------------------------
	Switch name/template:	"copyright=%s"
	Description:				Copyright message
	Flags:						String
	Default:						"(c) 1992 Adept Creations.  All Rights Reserved."
