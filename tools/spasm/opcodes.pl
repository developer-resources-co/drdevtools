
while( <> )
	{
  	# Immediate                   ADC #const    69     2*  2(1)
##	if( /.+([0-9A-F][0-9A-F])\s+[0-9].+([0-9])[\(\n]/ )
#	if( /.+([0-9A-F][0-9A-F])\s+\d.+(\d)[\(\n]/ )
	if( /.+([0-9A-F][0-9A-F])\s+\d.+(\d)[\(\n]/ )
		{
		($opcode_val,$time) = ($1,$2);
		chop;
		if( /\d\((.+)\)$/ )
			{
			$timings{ $opcode_val } = $1;
			}
		$opcode{ $opcode_val } = $time;
		$line{ $opcode_val } = $_;
		}
	}

foreach $i (sort keys( %opcode )  )
	{
	print "\tdb ",$opcode{ $i },"\t;",$line{$i},"\n";
	}

foreach $i (sort keys( %opcode ) )
	{
	print "\tdb 0";
	$_ = $timings{$i};
	while( s/(\d),*(\d*.*)$/$2/ )
		{                  
		print " OR (1 SHL (",$1,"-1))";
		}
	print "\n";
	}
