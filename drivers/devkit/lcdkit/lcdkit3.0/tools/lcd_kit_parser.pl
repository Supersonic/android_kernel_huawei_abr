#!/usr/bin/perl -w

#get the input target platform name
my $target_plat_name = shift;
my $target_file_type = shift;
#Execution start time
my $parser_start_time = time;

my $version_sub_path = "product_config/devkit_config";

if ($target_plat_name eq ""){
    print("The target platform name doesn't input!\n");
    exit 1;
}
#debug_print("The target plat you input is: $target_plat_name \n");

my @platform_chip_map = (
            [ "bengal",                 "qcom",          "bengal"     ],
            [ "kona",                 	"qcom",          "kona"       ],
            [ "lahaina",                "qcom",          "lahaina"    ],
            [ "hi6250",                 "hisi",          "hi6250"     ],
            [ "hi3660",                 "hisi",          "hi3660"     ],
            [ "kirin970",               "hisi",          "kirin970"   ],
            [ "kirin980",               "hisi",          "kirin980"   ],
            [ "kirin990",               "hisi",          "kirin990"   ],
            [ "baltimore",              "hisi",          "baltimore"  ],
            [ "denver",                 "hisi",          "denver"     ],
            [ "laguna",                 "hisi",          "laguna"     ],
            [ "burbank",                "hisi",          "burbank"    ],
            [ "orlando",                "hisi",          "orlando"    ],
            [ "k39tv1_64_bsp",          "mtk",           "k39tv1_64_bsp"],
            [ "k61v1_32_mexico",        "mtk",           "k61v1_64_mexico"],
            [ "k62v1_32_mexico",        "mtk",           "k61v1_64_mexico"],
            [ "k61v1_64_mexico",        "mtk",           "k61v1_64_mexico"],
            [ "k62v1_64_mexico",        "mtk",           "k62v1_64_mexico"],
            [ "k62v1_64_mex_a32",       "mtk",           "k62v1_64_mex_a32"],
            [ "k69v1_64",               "mtk",           "k69v1_64"],
            [ "tb8768p1_64_a32",        "mtk",           "tb8768p1_64_a32"],
            [ "k6873v1_64",             "mtk",           "k6873v1_64" ],
            [ "k6853v1_64",             "mtk",           "k6853v1_64" ],
            [ "k6889v1_64",             "mtk",           "k6889v1_64" ]);

#get plat name by parse chip name, so the chip name must be 'hixxxx' or 'msmxxxx'
my $chip_plat = '';
my $chip_name = '';
for (my $count = 0; $count < @platform_chip_map; $count++) {
    if ($target_plat_name eq $platform_chip_map[$count][0]) {
        print("The target plat you input match list: $count \n");
        $chip_name = $platform_chip_map[$count][2];
        $chip_plat = $platform_chip_map[$count][1];
    }
}

if (($chip_plat ne "qcom") && ($chip_plat ne "hisi") && ($chip_plat ne "mtk")){
    print("The chip plat: $chip_plat you input is error!\n");
    exit 1;
}

require "./lcd_kit_base_lib.pl";
require "./lcd_kit_property_items.pl";

debug_print("get parser parameter is: $chip_plat $chip_name $target_file_type\n");

my $parse_error_string = get_err_string();
debug_print("get error string is : $parse_error_string!\n");

if (($target_file_type ne "dtsi") && ($target_file_type ne "trebledto") && ($target_file_type ne "head")
 && ($target_file_type ne "dto-head") && ($target_file_type ne "effect") && ($target_file_type ne "all")
 && ($target_file_type ne "trebledts") && ($target_file_type ne "fwdtb"))
{
    error_print("The target file type doesn't input or input is error!\n");
    exit 1;
}
debug_print("The target file type you input is: $target_file_type \n");

my $out_head_file_path = "";
my $out_effect_file_path = "";
my $target_dtsi_file_path = "";

if (($target_file_type eq "dtsi") || ($target_file_type eq "trebledto") || ($target_file_type eq "all")
 || ($target_file_type eq "trebledts") || ($target_file_type eq "fwdtb"))
{
    $target_dtsi_file_path = shift;
    debug_print("get dtsi file path: $target_dtsi_file_path\n");
}

if (($target_file_type eq "head") || ($target_file_type eq "dto-head") || ($target_file_type eq "all"))
{
    $out_head_file_path = shift;
    debug_print("get head file path: $out_head_file_path\n");
}

if (($target_file_type eq "effect") || ($target_file_type eq "all"))
{
    $out_effect_file_path = shift;
    debug_print("get effect file path: $out_effect_file_path\n");
}

# get the abs path for tools folder, the tool will execute in this folder
my $working_path = File::Spec->rel2abs($0);
debug_print("working in path $working_path\n");
$working_path = dirname($working_path);
debug_print("working in path $working_path\n");
chdir $working_path;

my $plat_path;
my $root_path = "$working_path/../../../../../../..";
if ($chip_plat eq "qcom")
{
    $plat_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/config/qcom/$chip_name/lcdkit";
}
if ($chip_plat eq "mtk")
{
    $plat_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/config/mtk/$chip_name/lcdkit";
}
if ($chip_plat eq "hisi")
{
    $plat_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/config/hisi/$chip_name/lcdkit";
}

debug_print("get chip plat xml file path is: $plat_path\n");

if (!-e "$plat_path"){
    error_print("chip platform default xml file path not exist!\n");
    exit 1;
}


#collect plat xmls
my $plat_file;
my @plat_xmls = get_hisi_xml_list("plat_xmls");
foreach $plat_file (@plat_xmls) {
    debug_print("get plat xml files $plat_file\n");
}

#collect lcd xmls
my $panel_xml_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/panel";
my @panel_xmls = get_hisi_xml_list("panel_xmls");
foreach my $panel_file (@panel_xmls) {
    debug_print("get panel xml files $panel_file\n");
}

my $out_dtsi_file_path;
my $vender_list_path;
my $udp_vender_list_path = "$root_path/device/hisi/customize/udp_lcdkit/lcdkit3.0/config/hisi/$chip_name";
my $devkit_tool_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/tools";

#clean lcd generate files
if (($target_file_type eq "dtsi") || ($target_file_type eq "trebledto") || ($target_file_type eq "all") || ($target_file_type eq "fwdtb"))
{
    if ($target_dtsi_file_path eq "")
    {
        if ($chip_plat eq "hisi")
        {
            $target_dtsi_file_path = "$root_path/kernel/linux-4.1/arch/arm64/boot/dts/auto-generate/$chip_name/lcdkit3.0";
        }
        else
        {
            $target_dtsi_file_path = "$root_path/kernel/msm-3.18/arch/arm64/boot/dts/qcom/lcdkit3.0";
        }
    }
    
    if (-e "$target_dtsi_file_path"){
        debug_print("target dtsi file path exist, delete and create it now!\n");
        system("rm -f -r " . $target_dtsi_file_path);
    }
    system("mkdir -p " . $target_dtsi_file_path);
    $out_dtsi_file_path = "$target_dtsi_file_path/dtsi";
    if (!-e "$out_dtsi_file_path"){
        debug_print("out dtsi file path not exist, create it now!\n");
        system("mkdir " . $out_dtsi_file_path);
    }

    my @file_temp = glob("$out_dtsi_file_path/*.dtsi");
    for (@file_temp) {    unlink $_;    }
}

if (($target_file_type eq "head") || ($target_file_type eq "dto-head") || ($target_file_type eq "all"))
{
    if ($out_head_file_path eq "")
    {
        $out_head_file_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/panel/head";
    }
  
    if (!-e "$out_head_file_path"){
        debug_print("out head file path not exist, create it now!\n");
        system("mkdir -p " . $out_head_file_path);
    }
    
    @file_temp = glob("$out_head_file_path/*.h");
    for (@file_temp) {
        #unlink $_;
    }
}

if ((($target_file_type eq "effect") || ($target_file_type eq "all")) && ($chip_plat eq "hisi"))
{
    if ($out_effect_file_path eq "")
    {
        $out_effect_file_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/panel/effect";
    }
    if (!-e "$out_effect_file_path"){
        debug_print("out effect file path not exist, create it now!\n");
        system("mkdir -p " . $out_effect_file_path);
    }
    
    @file_temp = glob("$out_effect_file_path/*.h");
    for (@file_temp) {    unlink $_;    }
}

if ($chip_plat eq "qcom")
{
    $vender_list_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/config/qcom/$chip_name";
}

if ($chip_plat eq "mtk")
{
    $vender_list_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/config/mtk/$chip_name";
}

if ($chip_plat eq "hisi")
{
    $vender_list_path = "$root_path/vendor/huawei/chipset_common/devkit/lcdkit/lcdkit3.0/config/hisi/$chip_name";
}

#get lcd list
my @lcd_map_group;
my @lcd_effect_map_group;
my @lcd_default_panel_map_group;
my @lcd_product_map_group;

my @lcd_map_list = get_hisi_xml_list("lcd_map_list");
if (@lcd_map_list eq 0){
    error_print("cann't get lcd map list!\n");
    exit 1;
}

my @product_names;
my @lcd_file_names;
#hisi effect add
my $lcd_xml_parser = new XML::LibXML;

#ParseEffectFromDts's value from platform.xml
# 0: Parse effect parameter from effect head files.
# 1: Parse effect parameter from dts file, and will generate empty effect head files.
my $parse_effect_from_dts = parse_effect_from_dts(\@plat_xmls);
print("ParseEffectFromDts : $parse_effect_from_dts\n");

foreach my $list_entry (@lcd_map_list) {
    debug_print("get lcd map entry: $list_entry\n");

    $list_entry =~ s/ //g;
    $list_entry =~ s/\t//g;
    $list_entry =~ s/\n//g;
    my @lcd_element = split /,/, $list_entry;

    my $lcd_name    = $lcd_element[0];
    my $lcd_gpio    = $lcd_element[1];
    my $lcd_bdid    = $lcd_element[2];
    my $product     = $lcd_element[3];
    my $vender_fd   = $lcd_element[4];
    my $version_fd  = $lcd_element[5];

    debug_print("parsing: $lcd_name $lcd_gpio $lcd_bdid $product $vender_fd $version_fd\n");

    #get product list
    my $product_num = 0;
    $product =~ s/-/_/g;
    $product = lc($product);
    foreach my $product_name (@product_names) {
        debug_print("$product_name : $product \n");
        if ($product_name eq $product) {
            $product_num++;
        }
    }
    
    if ($product_num eq 0) {
        debug_print("push : $product \n");
        push(@product_names, $product);
    }

    my @parse_xml_files;

    if ($vender_fd ne 'def')
    {
        #collect version xmls
        if ($version_fd ne 'def')
        {
            my @version_xmls = get_hisi_xml_list("version_xmls", $vender_fd, $version_fd);
            foreach my $version_file (@version_xmls) {
                debug_print("get version xml files $version_file\n");
            }
            my $version_xml = get_xml_file(\@version_xmls, $lcd_name . '.xml');
            if ($version_xml ne $parse_error_string) {
                push(@parse_xml_files, $version_xml);
            }
            else
            {
                error_print("get version $lcd_name xml file failed!\n");
            }
        }

        #collect vender xmls
        my @vender_xmls = get_hisi_xml_list("vender_xmls", $vender_fd);
        foreach my $vender_file (@vender_xmls) {
            debug_print("get vender xml files $vender_file\n");
        }

        my $vender_xml = get_xml_file(\@vender_xmls, $lcd_name . '.xml');
        if ($vender_xml ne $parse_error_string) {
            push(@parse_xml_files, $vender_xml);
        }
        else
        {
            debug_print("get product $lcd_name xml file failed!\n");
        }
    }

    my $plat_xml = get_xml_file(\@plat_xmls, '\w+' . 'platform.xml');
    if ($plat_xml ne $parse_error_string) {
        push(@parse_xml_files, $plat_xml);
    }
    else
    {
        error_print("get platform $lcd_name xml file failed!\n");
    }

    my $panel_xml = get_xml_file(\@panel_xmls, $lcd_name . '.xml');
    my $xmldoc = $lcd_xml_parser->parse_file($panel_xml);
    my $compatible = "";
    for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/PanelCompatible')) {
                $compatible = $property->textContent();
    }
    if ($vender_fd ne 'def')
    {
        my @vender_xmls = get_hisi_xml_list("vender_xmls", $vender_fd);
        my $vender_xml = get_xml_file(\@vender_xmls, $lcd_name . '.xml');
        $xmldoc = $lcd_xml_parser->parse_file($vender_xml);
        for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/PanelCompatible')) {
            $compatible = $property->textContent();
        }
    }
    if ($panel_xml ne $parse_error_string) {
        push(@parse_xml_files, $panel_xml);
    }
    else
    {
        error_print("get panel $lcd_name xml file failed!\n");
    }

    if (@parse_xml_files eq 0) {
        error_print("get lcd xml files fail!\n");
        exit 1;
    }

    $product =~ s/-/_/g;
    $lcd_name =~ s/-/_/g;
    my $file_name = get_file_name($product, $lcd_name);

    if (($target_file_type eq "dtsi") || ($target_file_type eq "trebledto") || ($target_file_type eq "all") || ($target_file_type eq "fwdtb"))
    {
        my $dtsi_file_path = get_out_dtsi_path($out_dtsi_file_path, $file_name);

        my $parser_para_string = $chip_plat . ' ' . $target_file_type . ' ' . $parse_effect_from_dts . ' '
                            . $dtsi_file_path . ' ' . get_parse_xml_para(\@parse_xml_files);

        system("perl $devkit_tool_path/lcd_kit_dtsi_parser.pl " . $parser_para_string);
    }
#genarate platform effect file
	my @lcd_xml_doc;
	$file_name =~ s/-/_/g;
    if (($target_file_type eq "head") || ($target_file_type eq "dto-head") || ($target_file_type eq "all"))
    {
        my $head_file_path = get_out_head_path($out_head_file_path, $file_name);

        my $parser_para_string = $chip_plat . ' ' . $target_file_type . ' '
                            . $head_file_path . ' ' . get_parse_xml_para(\@parse_xml_files);

        #system("perl $devkit_tool_path/lcd_kit_head_parser.pl " . $parser_para_string);
    }
    
    if ((($target_file_type eq "effect") || ($target_file_type eq "all")) && ($chip_plat eq "hisi"))
    {
        my $effect_file_path = get_out_effect_path($out_effect_file_path, $file_name);

        my $parser_para_string = $chip_plat . ' '
                            . $effect_file_path . ' ' . get_parse_xml_para(\@parse_xml_files);

        gen_plat_effec_par($out_effect_file_path.'/plat_effect_para.h', $plat_xml);
        if(($parse_effect_from_dts ne "1") && (is_parse_effect($lcd_name, $vender_fd) == 1))
        {
            system("perl $devkit_tool_path/lcd_kit_effect_parser.pl " . $parser_para_string);
        }
    }
    #$file_name =~ s/-/_/g;
    #my @compatible_list = get_module_map_list("platform", \@plat_xmls, '/hwlcd/lcdlist/lcd');
    push(@lcd_file_names, $file_name);
    push(@lcd_map_group, $compatible.", "."\""."\/huawei,lcd_config/lcd_kit_".$file_name."\"".", ".$lcd_gpio.", ".$lcd_bdid);
    if (($lcd_gpio eq '0x02')||($lcd_gpio eq '0x0A'))
    {
        push(@lcd_default_panel_map_group, $compatible.", "."\""."\/huawei,lcd_config/lcd_kit_".$file_name."\"".", ".$lcd_gpio.", \"".$product."\"");
    }
    if ($lcd_bdid ne '0')
    {
        push(@lcd_product_map_group, $lcd_bdid.", \"".$product."\"");
    }
    #hisi effect add
    my $lcd_xml_files = $lcd_xml_parser->parse_file($panel_xml);
    my $xml_node = '/hwlcd/PanelEntry/PanelCompatible';
    my $parse_str = parse_single_xml($lcd_xml_files, $xml_node);
    push(@lcd_effect_map_group, uc($file_name)."_PANEL, ".$parse_str.", ".$lcd_bdid);
}

if (($target_file_type eq "head") || ($target_file_type eq "dto-head") || ($target_file_type eq "all"))
{
    print "=====================parsing head file: lcd_kit_panels.h ======================\n";
	
    my $out_head_file = "$out_head_file_path/lcd_kit_panels.h";
    open (my $lcd_head_file, '>'.$out_head_file) or die "open $out_head_file fail!\n";
    
    print $lcd_head_file create_file_header();
    print $lcd_head_file "#ifndef _LCDKIT_PANELS__H_\n";
    print $lcd_head_file "#define _LCDKIT_PANELS__H_\n\n";
    
    if ($chip_plat eq 'hisi')
    {
       # print $lcd_head_file "#include \"lcdkit_disp.h\"";
    }

    foreach my $file_name (@lcd_file_names) {
       # print $lcd_head_file "\n#include \"$file_name.h\"";
    }
    
    #print $lcd_head_file create_lcd_enum(\@lcd_file_names);
    print $lcd_head_file create_lcd_map_struct(\@lcd_map_group);
    print $lcd_head_file create_default_panel_map_struct(\@lcd_default_panel_map_group);
    print $lcd_head_file create_product_map_struct(\@lcd_product_map_group);
    #print $lcd_head_file create_data_init_func(\@lcd_file_names, $chip_plat);
    if ($chip_plat eq 'qcom')
    {
        print $lcd_head_file create_qcom_panel_init();
        
        my $default_id0_gpio = get_platform_attr("lcd", \@plat_xmls, '/hwlcd/PanelEntry/GpioId0');
        my $default_id1_gpio = get_platform_attr("lcd", \@plat_xmls, '/hwlcd/PanelEntry/GpioId1');
        my $default_id_gpio = $default_id0_gpio . "," . $default_id1_gpio;
        my $specical_id_gpio = get_platform_attr("lcd", \@plat_xmls, '/hwlcd/PanelEntry/SpecIdGpio');
        $specical_id_gpio = $specical_id_gpio ne $parse_error_string ? $specical_id_gpio : "\"0,0,0\"";
        
        print $lcd_head_file create_qcom_gpio_init($default_id_gpio, $specical_id_gpio);
    }
    else
    {
        #print $lcd_head_file create_hisi_panel_init();
    }
    print $lcd_head_file "\n#endif /*_HW_LCD_PANELS__H_*/\n";
    
    close($lcd_head_file);
}

if ((($target_file_type eq "effect") || ($target_file_type eq "all")) && ($chip_plat eq 'hisi'))
{
    print "=====================parsing effect file: lcd_kit_effect.h ======================\n";

    my $out_effect_file = "$out_effect_file_path/lcd_kit_effect.h";
    open (my $lcd_effect_file, '>'.$out_effect_file) or die "open $out_effect_file fail!\n";

    print $lcd_effect_file create_file_header();

    print $lcd_effect_file "#ifndef _LCDKIT_EFFECT__H_\n";
    print $lcd_effect_file "#define _LCDKIT_EFFECT__H_\n\n";
    print $lcd_effect_file "\n#include \"plat_effect_para.h\"\n";
    foreach my $list_entry (@lcd_map_list) {
        $list_entry =~ s/ //g;
        $list_entry =~ s/\t//g;
        $list_entry =~ s/\n//g;
        my @lcd_element = split /,/, $list_entry;
        my $lcd_name    = $lcd_element[0];
        my $product     = $lcd_element[3];
        my $vender_fd   = $lcd_element[4];
        $product = lc($product);
        if (($parse_effect_from_dts ne "1") && (is_parse_effect($lcd_name, $vender_fd) == 1))
        {
            $lcd_name =~ s/-/_/g;
            print $lcd_effect_file "\n#include \"$product"."_"."$lcd_name.h\"\n";
        }
    }
    #if parse_effect_from_dts is 1, generate empty effect head files.
    if ($parse_effect_from_dts ne "1") {
        print $lcd_effect_file create_lcd_enum(\@lcd_file_names);
        print $lcd_effect_file create_lcd_effect_map_struct(\@lcd_effect_map_group);
        print $lcd_effect_file create_lcd_effect_data_func(\@lcd_map_list);
        print $lcd_effect_file create_hisi_panel_effect_init();
    } else {
        print $lcd_effect_file create_lcd_effect_data_func_mock();
        print $lcd_effect_file create_hisi_panel_effect_init_mock();
    }
    print $lcd_effect_file "\n#endif /*_LCDKIT_EFFECT__H_*/\n";

    close($lcd_effect_file);
}

if (($target_file_type eq "effect") && (($chip_plat eq 'hisi') || ($chip_plat eq 'mtk') || ($chip_plat eq 'qcom')))
{
    print "=====================parsing dpd file: lcd_kit_dpd.h ======================\n";

    my $out_dpd_file = "$out_effect_file_path/lcd_kit_dpd.h";
    open (my $lcd_dpd_file, '>'.$out_dpd_file) or die "open $out_dpd_file fail!\n";

    print $lcd_dpd_file create_file_header();

    print $lcd_dpd_file "#ifndef _LCDKIT_DPD__H_\n";
    print $lcd_dpd_file "#define _LCDKIT_DPD__H_\n\n";

    print $lcd_dpd_file create_xml_name_map_struct();
    print $lcd_dpd_file "\n#endif /*_LCDKIT_DPD__H_*/\n";

    close($lcd_dpd_file);
}

if (($target_file_type eq "dtsi") || ($target_file_type eq "trebledto") || ($target_file_type eq "all") || ($target_file_type eq "fwdtb"))
{
    foreach my $product_name (@product_names) {

		my $target_out_dtsi_folder = "$target_dtsi_file_path/$product_name";
        if (!-e "$target_out_dtsi_folder"){
            debug_print("target out dtsi file folder not exist, create it now!\n");
            system("mkdir -p " . $target_out_dtsi_folder);
        }
    
        my @file_temp = glob("$target_out_dtsi_folder/*.dtsi");
        for (@file_temp) {    unlink $_;    }
    
        print "=====================parsing dtsi file: $product_name devkit_lcd_kit.dtsi ======================\n";
        my $out_dtsi_file = "$target_out_dtsi_folder/devkit_lcd_kit.dtsi";
        open (my $lcd_dtsi_file, '>'.$out_dtsi_file) or die "open $out_dtsi_file fail!\n";
        print $lcd_dtsi_file create_file_header();
        my @dtsi_files = glob("$out_dtsi_file_path/*.dtsi");
        foreach my $dtsi_file (@dtsi_files) {
            my $dtsi_name = $dtsi_file;
            $dtsi_name =~ s/$out_dtsi_file_path//g;
            $dtsi_name =~ s/\///g;
            my @file_element = split /-/, $dtsi_name;
            my $match_patten = $file_element[0];
            my $new_file_name = $dtsi_name;
            $new_file_name =~ s/-/_/g;

            if ($product_name eq $match_patten) {
                if ($chip_plat eq 'hisi')
                {
                    print $lcd_dtsi_file "\n/include/ \"$new_file_name\"";
                }
                else
                {
                    print $lcd_dtsi_file "\n#include \"$new_file_name\"";
                }
                
                system("cp -f " . "$out_dtsi_file_path/$dtsi_name $target_out_dtsi_folder/$new_file_name");
            }
        }
        close($lcd_dtsi_file);
    }
	system("rm -fr $out_dtsi_file_path");
}

if ($target_file_type eq "trebledts")
{
	my $out_dtsi_file = "$target_dtsi_file_path/devkit_lcd_kit.dtsi";
	open (my $lcd_dtsi_file, '>'.$out_dtsi_file) or die "open $out_dtsi_file fail!\n";
	print $lcd_dtsi_file "/ {\n";
	print $lcd_dtsi_file "\thuawei_lcd_config: huawei,lcd_config {\n";
	foreach my $list_entry (@lcd_map_list) {
		$list_entry =~ s/ //g;
		$list_entry =~ s/\t//g;
		$list_entry =~ s/\n//g;
		my @lcd_element = split /,/, $list_entry;
		my $lcd_name    = $lcd_element[0];
		my $product     = $lcd_element[3];
		$lcd_name =~ s/-/_/g;

		print $lcd_dtsi_file "\t\tdsi_" . lc($product) . "_" . lc($lcd_name) . ": lcd_kit_" . lc($product) . "_" . lc($lcd_name) . " {\n";
		print $lcd_dtsi_file "\t\t\tstatus = \"disabled\";\n";
		print $lcd_dtsi_file "\t\t};\n";
	}
	print $lcd_dtsi_file "\t};\n";
	print $lcd_dtsi_file "};\n";
	close($lcd_dtsi_file);
}
sub parse_plat_xml  {  return parse_multi_xml_base(\@lcd_xml_doc, shift);  }
sub parse_multi_xml  {  return parse_multi_xml_base(\@lcd_xml_doc, shift);  }
sub gen_plat_effec_par
{
	my $plat_name = "plat";
	my $plat_effect_file = shift;
	my @plat_effect_property_strings = (
		["gama lut R table",                              \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/GammaLutTableR",              \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_gamma_lut_table_R[] '                         ],
		["gama lut G table",                              \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/GammaLutTableG",              \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_gamma_lut_table_G[] '                         ],
		["gama lut B table",                              \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/GammaLutTableB",              \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_gamma_lut_table_B[] '                         ],
		["igm lut table R",                               \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/IgmLutTableR",                \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_igm_lut_table_R[] '                           ],
		["igm lut table G",                               \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/IgmLutTableG",                \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_igm_lut_table_G[] '                           ],
		["igm lut table B",                               \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/IgmLutTableB",                \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_igm_lut_table_B[] '                           ],
		["gama lut table low 32 bit",                     \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/GmpLutTableLow32bit",         \&create_effect_lut_grp,
		 'static u32 ' . $plat_name. '_gmp_lut_table_low32bit '             ],
		["gama lut table hign 4 bit",                     \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/GmpLutTableHigh4bit",         \&create_effect_lut_grp,
		 'static u32 ' . $plat_name. '_gmp_lut_table_high4bit '             ],
		["xcc table",                                     \&hfile_section_header,
		 ''                                                                                ],
		["/hwlcd/PanelEntry/XccTable",                    \&create_effect_lut_,
		 'static u32 ' . $plat_name. '_xcc_table[] '                                 ]);
	my $plat_xml = shift;
	push(@lcd_xml_doc, $lcd_xml_parser->parse_file($plat_xml));
	my $xml_version = parse_multi_xml('/hwlcd/Version');
	if(-e $plat_effect_file){
		debug_print("file exist\n");
	} else {
		open (my $plat_effect_file, '>'.$plat_effect_file);
		print $plat_effect_file create_hfile_header($xml_version, $plat_name);
		if ($parse_effect_from_dts ne "1") {
		    for ($count = 0; $count < @plat_effect_property_strings; $count++)  {
		        my $parser_tmp_str = $plat_effect_property_strings[$count][1]->($plat_effect_property_strings[$count][0]);
		        print $plat_effect_file $plat_effect_property_strings[$count][2] . $parser_tmp_str;
		    }
		}
		print $plat_effect_file create_hfile_tail($plat_name);
		close($plat_effect_file);
	}
}

sub parse_effect_from_dts
{
   my $plat_xmls = shift;
   my $plat_xml = get_xml_file($plat_xmls, '\w+' . 'platform.xml');
   my $xmldoc = $lcd_xml_parser->parse_file($plat_xml);
   my $parse_flag = parse_single_xml($xmldoc, '/hwlcd/PanelEntry/ParseEffectFromDts');
    if ($parse_flag eq get_err_string()) {
        $parse_flag = "0";
    }
    return $parse_flag;
}

sub is_parse_effect
{
	my $exist = 0;
	my $lcd_name;
	my $vender_fd;
	my @vender_xmls;
	$lcd_name = shift;
	$vender_fd = shift;
	if ($vender_fd ne 'def')
	{
		@vender_xmls = get_hisi_xml_list("vender_xmls", $vender_fd);
		$lcd_name =~ s/_/-/g;
		my $vender_xml = get_xml_file(\@vender_xmls, $lcd_name . '.xml');
		$xmldoc = $lcd_xml_parser->parse_file($vender_xml);
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/GammaLutTableR')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/GammaLutTableG')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/GammaLutTableB')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/IgmLutTableR')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/IgmLutTableG')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/IgmLutTableB')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/GmpLutTableLow32bit')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/GmpLutTableHigh4bit')) {
			$exist = 1;
		}
		for my $property($xmldoc->findnodes('/hwlcd/PanelEntry/XccTable')) {
			$exist = 1;
		}
	}
	return $exist;
}

sub get_hisi_xml_list
{
	my $tag_xmls;
	my $vender_fd;
	my $version_fd;
	my @dst_list;
	$tag_xmls = shift;
	if ($chip_plat eq "hisi") {
		if ($tag_xmls eq "plat_xmls") {
			@dst_list = (glob("$plat_path/*.xml"),
				glob("$root_path/device/hisi/customize/udp_lcdkit/lcdkit3.0/config/hisi/$chip_name/lcdkit/*.xml"));
		}
		elsif ($tag_xmls eq "panel_xmls") {
			@dst_list = (glob("$panel_xml_path/*.xml"),
				glob("$root_path/device/hisi/customize/udp_lcdkit/lcdkit3.0/panel/*.xml"));
		}
		elsif ($tag_xmls eq "lcd_map_list") {
			@dst_list = (get_module_map_list("platform", \@plat_xmls, '/hwlcd/lcdlist/lcd'),
				get_module_map_list("udp_part", \@plat_xmls, '/hwlcd/lcdlist/lcd'));
		}
		elsif ($tag_xmls eq "vender_xmls") {
			$vender_fd = shift;
			@dst_list = (glob("$vender_list_path/$vender_fd/*.xml"),
				glob("$udp_vender_list_path/$vender_fd/*.xml"));
		}
		elsif ($tag_xmls eq "version_xmls") {
			$vender_fd = shift;
			$version_fd = shift;
			@dst_list = (glob("$vender_list_path/$vender_fd/$version_fd/*.xml"),
				glob("$udp_vender_list_path/$vender_fd/$version_fd/*.xml"));
		}
		else {
			error_print("hisi get xmls'type not exist!\n");
		}
	}
	else {
		if ($tag_xmls eq "plat_xmls") {
			@dst_list = glob("$plat_path/*.xml");
		}
		elsif ($tag_xmls eq "panel_xmls") {
			@dst_list = glob("$panel_xml_path/*.xml");
		}
		elsif ($tag_xmls eq "lcd_map_list") {
			@dst_list = get_module_map_list("platform", \@plat_xmls, '/hwlcd/lcdlist/lcd');
		}
		elsif ($chip_plat eq "qcom" && $tag_xmls eq "vender_xmls") {
			$vender_fd = shift;
			@dst_list = glob("$vender_list_path/$vender_fd/*.xml");
		}
		elsif ($chip_plat eq "mtk" && $tag_xmls eq "vender_xmls") {
			$vender_fd = shift;
			@dst_list = glob("$vender_list_path/$vender_fd/*.xml");
		}
		elsif ($tag_xmls eq "version_xmls") {
			$vender_fd = shift;
			$version_fd = shift;
			@dst_list = glob("$vender_list_path/$vender_fd/$version_fd/*.xml");
		}
		else {
			error_print("qcom or mtk get xmls'type not exist!\n");
		}
	}
	return @dst_list;
}

exit 0;
