#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# Copyright (c) 2019 Huawei Technologies Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
#
# Merge lcd xml parameters files into one file.
#

import os
import glob
import shutil
import xml.dom.minidom


# get plat name
def get_plat_name(config_path_name):
    plat_list = [f for f in os.listdir(config_path_name) if not os.path.isfile(os.path.join(config_path_name, f))]
    plat_list.sort()
    for i in range(len(plat_list)):
        print("{}:{}".format(i, plat_list[i]))
    plat_index = input("Please input plat index(e.g.,1): ")
    plat_name = plat_list[int(plat_index)]
    return plat_name


# get chip name
def get_chip_name(chip_path):
    chip_list = [f for f in os.listdir(chip_path) if not os.path.isfile(os.path.join(chip_path, f))]
    chip_list.sort()
    for i in range(len(chip_list)):
        print("{}:{}".format(i, chip_list[i]))
    chip_index = input("Please input chip index(e.g.,1): ")
    chip_name = chip_list[int(chip_index)]
    return chip_name


# get product name
def get_product_name(product_path):
    product_list = [f for f in os.listdir(product_path) if not os.path.isfile(os.path.join(product_path, f))]
    product_list.pop(product_list.index("lcdkit"))
    product_list.sort()
    for i in range(len(product_list)):
        print("{}:{}".format(i, product_list[i]))
    product_index = input("Please input product index(e.g.,1): ")
    product_name = product_list[int(product_index)]
    return product_name


# get subproduct name(e.g. v4xml, vn1)
def get_subproduct_name(subproduct_path):
    subproduct_list = [f for f in os.listdir(subproduct_path) if not os.path.isfile(os.path.join(subproduct_path, f))]
    return subproduct_list


# make out dir
def mkdir_out(dirname, product_name, subproduct_list):
    if os.path.exists(dirname):
        shutil.rmtree(dirname)
    out_file_path = dirname + "/" + product_name + "/"
    os.makedirs(out_file_path)
    for i in subproduct_list:
        out_sub_path = "{}{}{}".format(out_file_path, i, "/")
        os.mkdir(out_sub_path)
    return out_file_path


# get PanelEntry sub node from product xml
def get_product_xml_node(product_PanelEntry, processed_node_list, out_file_path):
    out_xml_file = None
    product_childNodes = product_PanelEntry[0].childNodes
    for i in product_childNodes:
        if i.nodeType == xml.dom.Node.ELEMENT_NODE:
            if i.nodeName == "PanelCompatible":
                temp_sting = i.firstChild.data
                out_xml_file_name = "{}{}".format(temp_sting.replace("\"", ""), ".xml")
                out_xml_file = out_file_path + out_xml_file_name
            if i.nodeName in effect_para_list:
                product_PanelEntry[0].removeChild(i)
            else:
                processed_node_list.append(i.nodeName)
    return out_xml_file


# get PanelEntry sub node from subproduct xml
def get_subproduct_xml_node(subproduct_xmldoc, processed_node_list, product_PanelEntry, out_file_path, out_xml_file):
    subproduct_PanelEntry = subproduct_xmldoc.getElementsByTagName("PanelEntry")
    subproduct_childNodes = subproduct_PanelEntry[0].childNodes
    for i in subproduct_childNodes:
        if i.nodeType == xml.dom.Node.ELEMENT_NODE:
            if i.nodeName == "PanelCompatible":
                temp_sting = i.firstChild.data
                out_xml_file_name = "{}{}".format(temp_sting.replace("\"", ""), ".xml")
                out_xml_file = out_file_path + out_xml_file_name
            if i.nodeName in effect_para_list:
                continue
            elif i.nodeName in processed_node_list:
                childNodes = product_PanelEntry[0].getElementsByTagName(i.nodeName)
                childNodes[0].firstChild.nodeValue = i.firstChild.nodeValue
            else:
                product_PanelEntry[0].appendChild(i)
                processed_node_list.append(i.nodeName)
    return out_xml_file


# get PanelEntry sub node from plat xml
def get_plat_xml_node(plat_xmldoc, processed_node_list, product_PanelEntry):
    plat_PanelEntry = plat_xmldoc.getElementsByTagName("PanelEntry")
    plat_childNodes = plat_PanelEntry[0].childNodes
    for i in plat_childNodes:
        if i.nodeType == xml.dom.Node.ELEMENT_NODE:
            if i.nodeName in processed_node_list or i.nodeName in effect_para_list:
                continue
            else:
                product_PanelEntry[0].appendChild(i)
                processed_node_list.append(i.nodeName)


# get PanelEntry sub node from panel xml
def get_panel_xml_node(panel_xmldoc, processed_node_list, product_PanelEntry, out_file_path, out_xml_file):
    panel_PanelEntry = panel_xmldoc.getElementsByTagName("PanelEntry")
    panel_childNodes = panel_PanelEntry[0].childNodes
    for i in panel_childNodes:
        if i.nodeType == xml.dom.Node.ELEMENT_NODE:
            if not out_xml_file and i.nodeName == "PanelCompatible":
                temp_sting = i.firstChild.data
                out_xml_file_name = "{}{}".format(temp_sting.replace("\"", ""), ".xml")
                out_xml_file = out_file_path + out_xml_file_name
            if i.nodeName in processed_node_list:
                continue
            else:
                product_PanelEntry[0].appendChild(i)
                processed_node_list.append(i.nodeName)
    return out_xml_file


# write merged xml node to out file
def write_to_out_file(product_xmldoc, out_xml_file):
    temp_xml_name = "temp_xml_file"
    with open(temp_xml_name, "w+") as f0:
        f0.write(product_xmldoc.toprettyxml())
    with open(temp_xml_name, "r") as f1:
        if not out_xml_file:
            raise Exception("Can't find \"PanelCompatible\" node, please check.")
        with open(out_xml_file, "w+") as f2:
            for i in f1:
                temp = i.strip()
                if temp:
                    f2.write(i.replace("&quot;", "\""))
    if os.path.exists(temp_xml_name):
        os.remove(temp_xml_name)


def print_out_xml_list(out_xml_list):
    print("\n===================== out xml file path ==============================")
    out_xml_list.sort()
    for i in out_xml_list:
        print(i)
    print("======================================================================\n")


# merge product folder xml files(e.g. Mars/, not Mars/V4/)
def merge_product_xml(product_xml_path, plat_xml_file, out_file_path, out_xml_list):
    product_xml_list = glob.glob(product_xml_path + '/*.xml')
    for product_xml_file in product_xml_list:
        print("Processing: {}".format(product_xml_file))
        processed_node_list = []
        product_xml_temp = product_xml_file.split("/")
        xml_name = product_xml_temp[len(product_xml_temp)-1]
        panel_xml_file = os.path.join(panel_xml_path, xml_name)

        product_xmldoc = xml.dom.minidom.parse(product_xml_file)
        product_PanelEntry = product_xmldoc.getElementsByTagName("PanelEntry")
        panel_xmldoc = xml.dom.minidom.parse(panel_xml_file)
        plat_xmldoc = xml.dom.minidom.parse(plat_xml_file)

        out_xml_file = get_product_xml_node(product_PanelEntry, processed_node_list, out_file_path)
        get_plat_xml_node(plat_xmldoc, processed_node_list, product_PanelEntry)
        out_xml_file = get_panel_xml_node(panel_xmldoc, processed_node_list, product_PanelEntry,
                                                                out_file_path, out_xml_file)

        write_to_out_file(product_xmldoc, out_xml_file)
        product_xmldoc.unlink()
        plat_xmldoc.unlink()
        panel_xmldoc.unlink()
        out_xml_list.append(out_xml_file)


# merge subproduct folder xml files(e.g. Mars/V4/)
def merge_subproduct_xml(product_xml_path, subproduct_list, plat_xml_file, out_file_path, out_xml_list):
    for subproduct_name in subproduct_list:
        subproduct_xml_path = os.path.join(product_xml_path, subproduct_name)
        sub_out_file_path = "{}{}{}".format(out_file_path, subproduct_name, "/")
        product_xml_list = glob.glob(os.path.join(product_xml_path, '*.xml'))
        for product_xml_file in product_xml_list:
            processed_node_list = []
            product_xml_temp = product_xml_file.split("/")
            xml_name = product_xml_temp[len(product_xml_temp) - 1]
            product_xml_file = os.path.join(product_xml_path, xml_name)
            panel_xml_file = os.path.join(panel_xml_path, xml_name)
            subporoduct_xml_file = os.path.join(subproduct_xml_path, xml_name)
            print("Processing: {}".format(subporoduct_xml_file))

            product_xmldoc = xml.dom.minidom.parse(product_xml_file)
            product_PanelEntry = product_xmldoc.getElementsByTagName("PanelEntry")
            panel_xmldoc = xml.dom.minidom.parse(panel_xml_file)
            plat_xmldoc = xml.dom.minidom.parse(plat_xml_file)

            out_xml_file = get_product_xml_node(product_PanelEntry, processed_node_list, sub_out_file_path)
            if os.path.exists(subporoduct_xml_file):
                subproduct_xmldoc = xml.dom.minidom.parse(subporoduct_xml_file)
                out_xml_file = get_subproduct_xml_node(subproduct_xmldoc, processed_node_list, product_PanelEntry,
                                                   sub_out_file_path, out_xml_file)
            get_plat_xml_node(plat_xmldoc, processed_node_list, product_PanelEntry)
            out_xml_file = get_panel_xml_node(panel_xmldoc, processed_node_list, product_PanelEntry,
                                              sub_out_file_path, out_xml_file)

            write_to_out_file(product_xmldoc, out_xml_file)
            product_xmldoc.unlink()
            plat_xmldoc.unlink()
            panel_xmldoc.unlink()
            out_xml_list.append(out_xml_file)


# merge xml file
def merge_xml(product_xml_path, subproduct_list, plat_xml_file, out_file_path):
    out_xml_list = []
    merge_product_xml(product_xml_path, plat_xml_file, out_file_path, out_xml_list)
    merge_subproduct_xml(product_xml_path, subproduct_list, plat_xml_file, out_file_path, out_xml_list)
    print_out_xml_list(out_xml_list)


# global parameters
effect_para_list = ["AcmLutHueTable", "AcmLutSataTable", "AcmLutSatrTable", "AcmLutSatr0Table",
                    "AcmLutSatr1Table", "AcmLutSatr2Table", "AcmLutSatr3Table", "AcmLutSatr4Table",
                    "AcmLutSatr5Table", "AcmLutSatr6Table", "AcmLutSatr7Table", "VideoAcmLutHueTable",
                    "VideoAcmLutSataTable", "VideoAcmLutSatr0Table", "VideoAcmLutSatr1Table", "VideoAcmLutSatr2Table",
                    "VideoAcmLutSatr3Table", "VideoAcmLutSatr4Table", "VideoAcmLutSatr5Table", "VideoAcmLutSatr6Table",
                    "VideoAcmLutSatr7Table", "GammaLutTableR", "GammaLutTableG", "GammaLutTableB", "IgmLutTableR",
                    "IgmLutTableG", "IgmLutTableB", "GmpLutTableLow32bit", "GmpLutTableHigh4bit", "XccTable"]
config_path = "../config/"
out_dirname = "out"
panel_xml_path = "../panel/"


# main func, read code from here.
def main():
    plat_name = get_plat_name(config_path)
    chip_path = config_path + plat_name
    chip_name = get_chip_name(chip_path)
    product_path = chip_path + "/" + chip_name
    product_name = get_product_name(product_path)
    subproduct_path = product_path + "/" + product_name
    subproduct_list = get_subproduct_name(subproduct_path)
    print("\nYour input parameters are: " + plat_name, chip_name, product_name + "\n")

    out_file_path = mkdir_out(out_dirname, product_name, subproduct_list)
    plat_xml_file = product_path + "/lcdkit/lcd_kit_platform.xml"
    product_xml_path = product_path + "/" + product_name
    merge_xml(product_xml_path, subproduct_list, plat_xml_file, out_file_path)


if __name__ == "__main__":
    main()
