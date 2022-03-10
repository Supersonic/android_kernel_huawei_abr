# lcdkit-effect-parser.pl


my $paranum = 0;
my $platform = $ARGV[$paranum];
$paranum++;

if ($platform eq 'qcom')
{
    require "./lcdkit-base-lib.pl";
}
else
{
    require "./lcd_kit_base_lib.pl";
}

# initialize the parser
my $lcd_xml_parser = new XML::LibXML;
my $parse_error_string = get_err_string();

debug_print("get chip platform is : $platform\n");

my $out_effect_file = $ARGV[$paranum];
$paranum++;
debug_print("get effect file path is : $out_effect_file\n");

my $xml_file_num = uc($ARGV[$paranum]);
$paranum++;
debug_print("get xml file number is $xml_file_num\n");

if(scalar @ARGV < ($xml_file_num + $paranum))
{
    error_print(($xml_file_num + $paranum) . " command line arguments required.\n");
    error_print("1-chip platform(hisi or qcom)\n");
    error_print("2-out file path of effect head file\n");
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

my $panel_file_name = file_name_from_path($out_effect_file);
welcome_print_begin(\@lcd_xml_files, $panel_file_name);

my $xml_version = parse_multi_xml('/hwlcd/Version');
if ($xml_version eq $parse_error_string) {
    error_print("get error xml file version!\n");
    exit_with_info($panel_file_name, 1);
}
debug_print("xml file version is : $xml_version\n");

print "=====================parsing effect file: $panel_file_name.h ======================\n";

#generater effect head file used in kernel by hisi platform
open (my $panel_effect_file, '>'.$out_effect_file) or die "fail to open file $out_effect_file\n";

print $panel_effect_file create_hfile_header($xml_version, $panel_file_name);

my @effect_property_strings = (
    ["gama lut R table",                              \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/GammaLutTableR",              \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_gamma_lut_table_R[] '                         ],
    ["gama lut G table",                              \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/GammaLutTableG",              \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_gamma_lut_table_G[] '                         ],
    ["gama lut B table",                              \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/GammaLutTableB",              \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_gamma_lut_table_B[] '                         ],
    ["igm lut table R",                               \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/IgmLutTableR",                \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_igm_lut_table_R[] '                           ],
    ["igm lut table G",                               \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/IgmLutTableG",                \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_igm_lut_table_G[] '                           ],
    ["igm lut table B",                               \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/IgmLutTableB",                \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_igm_lut_table_B[] '                           ],
    ["gama lut table low 32 bit",                     \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/GmpLutTableLow32bit",         \&create_effect_lut_grp,
     'static u32 ' . $panel_file_name. '_gmp_lut_table_low32bit '             ],
    ["gama lut table hign 4 bit",                     \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/GmpLutTableHigh4bit",         \&create_effect_lut_grp,
     'static u32 ' . $panel_file_name. '_gmp_lut_table_high4bit '             ],
    ["xcc table",                                     \&hfile_section_header,
     ''                                                                                ],
    ["/hwlcd/PanelEntry/XccTable",                    \&create_effect_lut,
     'static u32 ' . $panel_file_name. '_xcc_table[] '                                 ]);

for ($count = 0; $count < @effect_property_strings; $count++)  {	
    my $parser_tmp_str = $effect_property_strings[$count][1]->($effect_property_strings[$count][0]);
    print $panel_effect_file $effect_property_strings[$count][2] . $parser_tmp_str;
}

print $panel_effect_file create_hfile_tail($panel_file_name);
close($panel_effect_file);

exit_with_info($panel_file_name, 0);


sub parse_multi_xml  {  return parse_multi_xml_base(\@lcd_xml_doc, shift);  }
