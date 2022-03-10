# lcdkit-dtsi-parser.pl

my $paranum = 0;
my $platform = $ARGV[$paranum];
$paranum++;
my $target_file_type = $ARGV[$paranum];
$paranum++;
my $parse_effect_from_dts = $ARGV[$paranum];
$paranum++;

require "./lcd_kit_base_lib.pl";
require "./lcd_kit_property_items.pl";

# initialize the parser
my $lcd_xml_parser = new XML::LibXML;
my $parse_error_string = get_err_string();

debug_print("get chip platform is : $platform\n");
my $out_dtsi_file = $ARGV[$paranum];
$paranum++;
debug_print("get dtsi file path is : $out_dtsi_file\n");

my $xml_file_num = uc($ARGV[$paranum]);
$paranum++;
debug_print("get xml file number is $xml_file_num\n");

if(scalar @ARGV < ($xml_file_num + $paranum))
{
    error_print(($xml_file_num + $paranum) . " command line arguments required.\n");
    error_print("1-chip platform(hisi or qcom)\n");
    error_print("2-out file path of dtsi file\n");
    error_print("3-the number of XML Document to be parsed\n");
    error_print("4-". ($xml_file_num + $paranum) . ":XML Documents to parse\n");

    exit 1;
}

my @lcd_xml_doc;
my @lcd_xml_files;
for (my $count = 0; $count < $xml_file_num; $count++) {
    debug_print("get para file [$count] is $ARGV[$count + $paranum]\n");
    push(@lcd_xml_files, $ARGV[$count + $paranum]);
    push(@lcd_xml_doc, $lcd_xml_parser->parse_file($ARGV[$count + $paranum]));
}

my $panel_file_name = file_name_from_path($out_dtsi_file);
welcome_print_begin(\@lcd_xml_files, $panel_file_name);

my $xml_version = parse_multi_xml('/hwlcd/Version');
if ($xml_version eq $parse_error_string) {
    error_print("get error xml file version!\n");
    exit_with_info($panel_file_name, 1);
}
debug_print("xml file version is : $xml_version\n");

print "=====================parsing dtsi file: $panel_file_name.dtsi ======================\n";

#generater dtsi file used in kernel
open (my $panel_dtsi_file, '>'.$out_dtsi_file) or die "fail to open file $out_dtsi_file\n";

print $panel_dtsi_file create_dtsi_header($panel_file_name, $xml_version, $target_file_type, $platform);

#add entry if need, and please keep alignment, thanks!
my @dtsi_lcd_bias_ic_attrs = (
    ["BiasICName",                "-n",  "",  "",  ""],
	["BiasICType",                "-n",   0,  "lcdkit-bias-type = <",  ">;\n"],
	["BiasICVposReg",             "-n",   0,  "lcdkit-bias-vpos-reg = <",  ">;\n"],
	["BiasICVnegReg",             "-n",   0,  "lcdkit-bias-vneg-reg = <",  ">;\n"],
	["BiasICVposVal",             "-n",   0,  "lcdkit-bias-vpos-val = <",  ">;\n"],
	["BiasICVnegVal",             "-n",   0,  "lcdkit-bias-vneg-val = <",  ">;\n"],
	["BiasICVposMask",            "-n",   0,  "lcdkit-bias-vpos-mask = <",  ">;\n"],
	["BiasICVnegMask",            "-n",   0,  "lcdkit-bias-vneg-mask = <",  ">;\n"],
	["BiasICStateReg",            "-n",   0,  "lcdkit-bias-state-reg = <",  ">;\n"],
	["BiasICStateVal",            "-n",   0,  "lcdkit-bias-state-val = <",  ">;\n"],
	["BiasICStateMask",           "-n",   0,  "lcdkit-bias-state-mask = <",  ">;\n"]
);

if ($target_file_type eq "fwdtb") {
    for ($count = 0; $count < @fwdtb_dtsi_property_strings; $count++){
        if (($platform ne $fwdtb_dtsi_property_strings[$count][4])
         && ('comm' ne $fwdtb_dtsi_property_strings[$count][4]))
        {
            next;
        }

        if ($fwdtb_dtsi_property_strings[$count][0] eq 'need_nothing_here'){
            print $panel_dtsi_file $fwdtb_dtsi_property_strings[$count][5];
            next;
        }

        if ($fwdtb_dtsi_property_strings[$count][0] eq 'depond-on')
        {
            my $parser_tmp_str = $fwdtb_dtsi_property_strings[$count][1]->($fwdtb_dtsi_property_strings[$count][2]);
            if ($parser_tmp_str ne $parse_error_string){
                print $panel_dtsi_file $fwdtb_dtsi_property_strings[$count][5];
            }
            next;
        }

        my $parser_tmp_str = $fwdtb_dtsi_property_strings[$count][1]->($fwdtb_dtsi_property_strings[$count][0]);
        if ($parser_tmp_str eq $parse_error_string)    {
            debug_print("parse $fwdtb_dtsi_property_strings[$count][0] get a error string!\n");
            next;
        } elsif ($fwdtb_dtsi_property_strings[$count][0] =~ /Command$/) {
            check_command($parser_tmp_str);
        }

        if ($fwdtb_dtsi_property_strings[$count][3] eq '-f')    {
            $parser_tmp_str = $fwdtb_dtsi_property_strings[$count][2]->($parser_tmp_str);
            if ($parser_tmp_str eq $parse_error_string)    {
                debug_print("parse $fwdtb_dtsi_property_strings[$count][0] string trans error!\n");
                next;
            }
        }

        if ($fwdtb_dtsi_property_strings[$count][3] eq "eq")    {
            if ($parser_tmp_str ne $fwdtb_dtsi_property_strings[$count][2])    {
                next;
            }

            $parser_tmp_str = '';
        }

        if ($fwdtb_dtsi_property_strings[$count][3] eq "ne")    {
            if ($parser_tmp_str eq $fwdtb_dtsi_property_strings[$count][2])    {
                next;
            }

            $parser_tmp_str = '';
        }

        $parser_tmp_str = $fwdtb_dtsi_property_strings[$count][5].$parser_tmp_str.$fwdtb_dtsi_property_strings[$count][6];
        print $panel_dtsi_file $parser_tmp_str;
    }
} else {
    for ($count = 0; $count < @dtsi_property_strings; $count++){
        if (($platform ne $dtsi_property_strings[$count][4])
         && ('comm' ne $dtsi_property_strings[$count][4]))
        {
            next;
        }

        if ($parse_effect_from_dts ne "1") {
	        if ((index($dtsi_property_strings[$count][0], "GammaLutTableR") >= 0)
	            || (index($dtsi_property_strings[$count][0], "GammaLutTableG") >= 0)
	            || (index($dtsi_property_strings[$count][0], "GammaLutTableB") >= 0)
	            || (index($dtsi_property_strings[$count][0], "IgmLutTableR") >= 0)
	            || (index($dtsi_property_strings[$count][0], "IgmLutTableG") >= 0)
	            || (index($dtsi_property_strings[$count][0], "IgmLutTableB") >= 0)
	            || (index($dtsi_property_strings[$count][0], "GmpLutTableLow32bit") >= 0)
	            || (index($dtsi_property_strings[$count][0], "GmpLutTableHigh4bit") >= 0)
	            || (index($dtsi_property_strings[$count][0], "XccTable") >= 0)) {
	                print("ParseEffectFromDts != 1, ignore $dtsi_property_strings[$count][0] to dtsi\n");
	                next;
	        }
        }

        if ($dtsi_property_strings[$count][0] eq 'need_nothing_here'){
            print $panel_dtsi_file $dtsi_property_strings[$count][5];
            next;
        }

        if ($dtsi_property_strings[$count][0] eq 'depond-on')
        {
            my $parser_tmp_str = $dtsi_property_strings[$count][1]->($dtsi_property_strings[$count][2]);
            if ($parser_tmp_str ne $parse_error_string){
                print $panel_dtsi_file $dtsi_property_strings[$count][5];
            }
            next;
        }

        my $parser_tmp_str = $dtsi_property_strings[$count][1]->($dtsi_property_strings[$count][0]);
        if ($parser_tmp_str eq $parse_error_string)    {
            debug_print("parse $dtsi_property_strings[$count][0] get a error string!\n");
            next;
        } elsif ($dtsi_property_strings[$count][0] =~ /Command$/) {
            check_command($parser_tmp_str);
        }

        if ($dtsi_property_strings[$count][3] eq '-f')    {
            $parser_tmp_str = $dtsi_property_strings[$count][2]->($parser_tmp_str);
            if ($parser_tmp_str eq $parse_error_string)    {
                debug_print("parse $dtsi_property_strings[$count][0] string trans error!\n");
                next;
            }
        }

        if ($dtsi_property_strings[$count][3] eq "eq")    {
            if ($parser_tmp_str ne $dtsi_property_strings[$count][2])    {
                next;
            }

            $parser_tmp_str = '';
        }

        if ($dtsi_property_strings[$count][3] eq "ne")    {
            if ($parser_tmp_str eq $dtsi_property_strings[$count][2])    {
                next;
            }

            $parser_tmp_str = '';
        }

        $parser_tmp_str = $dtsi_property_strings[$count][5].$parser_tmp_str.$dtsi_property_strings[$count][6];
        print $panel_dtsi_file $parser_tmp_str;
    }
}
my @dtsi_lcd_backlight_ic_attrs = (
    ["BacklightICName",                "-n",    "",  "",  ""],
    ["BacklightICLevel",               "-n",     0,  "lcdkit-bl-ic-level = <", ">;\n"],
    ["BacklightICKernelBlCtrlMode",    "-n",     0,  "lcdkit-bl-ic-ctrl-mode = <", ">;\n"],
    ["BacklightICType",                "-n",     0,  "lcdkit-bl-ic-type = <", ">;\n"],
    ["BacklightICBeforeInitDelay",     "-n",     0,  "lcdkit-bl-ic-before-init-delay = <", ">;\n"],
    ["BacklightICInitDelay",           "-n",     0,  "lcdkit-bl-ic-init-delay = <", ">;\n"],
    ["BacklightICOVPCheck",            "-n",     0,  "lcdkit-bl-ic-ovp-check = <", ">;\n"],
    ["BacklightICNoLcdOVPCheck",       "-n",     0,  "lcdkit-bl-ic-fake-lcd-ovp-check = <", ">;\n"],
    ["BacklightICKernelInitCMD",       "-f",     0,  "lcdkit-bl-ic-init-cmd = <", ">;\n"],
    ["BacklightICNumOfKernelInitCMD",  "-n",     0,  "lcdkit-bl-ic-num-of-init-cmd = <", ">;\n"],
    ["BacklightICBLLSBRegCMD",         "-f",     0,  "lcdkit-bl-ic-bl-lsb-reg-cmd = <", ">;\n"],
    ["BacklightICBLMSBRegCMD",         "-f",     0,  "lcdkit-bl-ic-bl-msb-reg-cmd = <", ">;\n"],
    ["BacklightICBLEnableCMD",         "-f",     0,  "lcdkit-bl-ic-bl-enable-cmd = <", ">;\n"],
    ["BacklightICBLDisableCMD",        "-f",     0,  "lcdkit-bl-ic-bl-disable-cmd = <", ">;\n"],
    ["BacklightICDeviceDisableCMD",    "-f",     0,  "lcdkit-bl-ic-disable-device-cmd = <", ">;\n"],
    ["BacklightICFaultFlagCMD",        "-f",     0,  "lcdkit-bl-ic-fault-flag-cmd = <", ">;\n"],
    ["BacklightICBiasEnableCMD",       "-f",     0,  "lcdkit-bl-ic-bias-disable-cmd = <", ">;\n"],
    ["BacklightICBiasDisableCMD",      "-f",     0,  "lcdkit-bl-ic-bias-enable-cmd = <", ">;\n"],
    ["BacklightICOpenShortTest",       "-n",     0,  "lcdkit-bl-ic-led-open-short-test = <", ">;\n"],
    ["BacklightICNumOfLed",            "-n",     0,  "lcdkit-bl-ic-num-of-led = <", ">;\n"],
    ["BacklightICBrtCtrlCMD",          "-f",     0,  "lcdkit-bl-ic-brt-ctrl-cmd = <", ">;\n"],
    ["BacklightICFaultCtrlCMD",        "-f",     0,  "lcdkit-bl-ic-fault-ctrl-cmd = <", ">;\n"]
);
my $lcd_bias_ic_base_tag = "hwlcd/HWBIASICLIST/BIASIC";
my $lcd_bias_ic_name_tag = "BiasICName";
create_dtsi_struct_array($lcd_bias_ic_base_tag, \@dtsi_lcd_bias_ic_attrs, \@lcd_xml_doc, $lcd_bias_ic_name_tag);
my $lcd_backlight_ic_base_tag = "hwlcd/HWBACKLIGHTCLIST/BACKLIGHTIC";
my $lcd_backlight_ic_name_tag = "BacklightICName";
create_dtsi_struct_array($lcd_backlight_ic_base_tag, \@dtsi_lcd_backlight_ic_attrs, \@lcd_xml_doc, $lcd_backlight_ic_name_tag);

sub create_dtsi_struct_array
{
	my $nodebase = shift;
	my $attr_list = shift;
	my @attr_list = @$attr_list;
	my $xml_doc_list = shift;
	my @xml_doc_list = @$xml_doc_list;
	my $ic_name_tag = shift;
	my $parser_string = "";
	my $ic_name = "";

    foreach my $xml_doc (@xml_doc_list) {

    for my $dest($xml_doc->findnodes($nodebase)) {
		for (my $tmp_count = 0; $tmp_count < @attr_list; $tmp_count++) {
			$list_entry = $dest->findvalue('./' . $attr_list[$tmp_count][0]);
			if($attr_list[$tmp_count][0] eq $ic_name_tag)
			{
			    if($list_entry eq '')
				{
				    $ic_name = "default";
				}
				else
				{
				    $list_entry=~ s/"//g;
					$ic_name = "\t\t" . $list_entry;
				}
				next;
			}
			if ($list_entry eq '')
			{
			    if($attr_list[$tmp_count][1] eq "-f")
			    {
			        $list_entry = '0x00, 0x00, 0x00, 0x00';
			    }
			    else
			    {
			        $list_entry = $attr_list[$tmp_count][2];
			    }
			}
			$list_entry=~ s/"//g;
			$list_entry=~ s/,//g;
			$parser_string .= $ic_name . "," . $attr_list[$tmp_count][3] . $list_entry . $attr_list[$tmp_count][4];
		}
    }
	}

    print $panel_dtsi_file $parser_string;
}

if ($target_file_type eq "fwdtb")
{
    print $panel_dtsi_file create_fwdtb_dtsi_tail();
}
else
{
    print $panel_dtsi_file create_dtsi_tail();
}

close($panel_dtsi_file);

exit_with_info($panel_file_name, 0);

sub parser_hex_order {  return parser_hex_order_base(shift, $platform);  }
sub parser_hex_arrays { return parser_hex_arrays_base(shift, $platform); }
sub parse_multi_xml  {  return parse_multi_xml_base(\@lcd_xml_doc, shift);  }
sub parser_hex_order_ext {  return parser_hex_order_base_ext(shift);  }

sub check_command {
    my @commands = split(/\n/,$_[0]);

    for (my $count = 0; $count < @commands; $count++) {
        debug_print("command:$commands[$count]\n");
        my @command = split(/, /,$commands[$count]);
        foreach my $code_ele (@command) {
            $code_ele =~ tr/\t" //d;
        }

        #check code number
        if (@command < 2 && hex($command[0]) == 0) {
            next;
        } elsif (@command < 8) {
            error_print("in command:$commands[$count] -- code number error!\n");
            exit 1;
        }

        my $dtype = hex($command[0]);
        my $last = hex($command[1]);
        my $vc = hex($command[2]);
        my $ack = hex($command[3]);
        my $wait = hex($command[4]);
        my $waittype = hex($command[5]);
        my $dlen = hex($command[6]);

        debug_print("\$dtype:$dtype\n");
        debug_print("\$last:$last\n");
        debug_print("\$vc:$vc\n");
        debug_print("\$ack:$ack\n");
        debug_print("\$wait:$wait\n");
        debug_print("\$waittype:$waittype\n");
        debug_print("\$dlen:$dlen\n");

        #check dtype value
        if (0 == grep { $_ == $dtype } (0x05,0x07,0x15,0x06,0x39,0x0A,0x03,0x13,0x23,0x29,0x04,0x14,0x24)) {
            error_print("in command:$commands[$count] -- dtype error!\n");
            exit 1;
        }

        #check last value
        if (0 == grep { $_ == $last } (0x00,0x01)) {
            error_print("in command:$commands[$count] -- last error!\n");
            exit 1;
        }

        #check vc value
        if (0 == grep { $_ == $vc } (0x00,0x01,0x02,0x03)) {
            error_print("in command:$commands[$count] -- vc error!\n");
            exit 1;
        }

        #check ack value
        if (0 == grep { $_ == $ack } (0x00,0x01)) {
            error_print("in command:$commands[$count] -- ack error!\n");
            exit 1;
        }

        #check wait and dlen
        if ($wait < 0 || $dlen <= 0 || ($dlen!=@command-7 && dtype_is_read($dtype)==0)) {
            error_print("in command:$commands[$count] -- wait or dlen value not right!\n");
            exit 1;
        }

        #check waittype value
        if (0 == grep { $_ == $waittype } (0x00,0x01)) {
            error_print("in command:$commands[$count] -- waittype error!\n");
            exit 1;
        }

        #check payload value
        for (my $i=7; $i < @command; $i++) {
            if(hex($command[$i]) < 0 || hex($command[$i]) > 0xff){
                error_print("in command:$commands[$count] -- payload value not right!\n");
                exit 1;
            }
        }

        #check wait value -- wait(us) >= 1/9 * 8 * dlen
#        if (@command > 19 && dtype_is_read($dtype) == 0) {
#            my $waittime = $wait * (($waittype == 0x01) ? 1000 : 1);
#            if ($waittime < 1/9 * 8 * $dlen) {
#                error_print("in command:$commands[$count] -- wait time too short!\n");
#                exit 1;
#            }
#        }

        debug_print("check success.\n");
    }
}

sub dtype_is_read{
    if (grep { $_ == $_[0] } (0x06,0x04,0x14,0x24)) {
        return 1;
    }
    return 0;
}
