#!/bin/env python

import struct
import sys
import re
import xml.etree.ElementTree as ET

# Extract constants from header file
constants = {}
with open('jan/jan.h', 'r') as file:
    output = file.read()
    pattern = re.compile('[A-Z_]+')
    lines = output.split('\n')
    for line in lines:
        line = line.strip()
        if line.startswith('#define '):
            line = line[len('#define ' ):]
            if (
                line.startswith('JAN_TYPE_') or line.startswith('JAN_ATTRIBUTE_') or
                line.startswith('JAN_ORIENTATION_') or line.startswith('JAN_ALIGN_') or
                line.startswith('JAN_FONT_WEIGHT_') or line.startswith('JAN_UNIT_')
            ):
                line = line.split(' ')
                rest = line[1:]
                for i, item in enumerate(rest):
                    if pattern.fullmatch(item):
                        rest[i] = 'constants["' + item + '"]'
                constants[line[0]] = eval(' '.join(rest))

# Scan XML
tree = ET.parse(sys.argv[1])
root = tree.getroot()
with open (sys.argv[2], 'wb') as file:
    def parseUnit(value):
        value = value.strip()

        if value == '0':
            return (0, constants['JAN_UNIT_TYPE_PX'])

        if value.endswith('px'):
            return (float(value[:-2]), constants['JAN_UNIT_TYPE_PX'])

        if value.endswith('dp'):
            return (float(value[:-2]), constants['JAN_UNIT_TYPE_DP'])

        if value.endswith('sp'):
            return (float(value[:-2]), constants['JAN_UNIT_TYPE_SP'])

        if value.endswith('vw'):
            return (float(value[:-2]), constants['JAN_UNIT_TYPE_VW'])

        if value.endswith('vh'):
            return (float(value[:-2]), constants['JAN_UNIT_TYPE_VH'])

        if value.endswith('vmin'):
            return (float(value[:-4]), constants['JAN_UNIT_TYPE_VMIN'])

        if value.endswith('vmax'):
            return (float(value[:-4]), constants['JAN_UNIT_TYPE_VMAX'])

        if value.endswith('%'):
            return (float(value[:-1]), constants['JAN_UNIT_TYPE_PERCENT'])

        if value == 'wrap':
            return (0, constants['JAN_UNIT_TYPE_WRAP'])

        if value == 'match':
            return (100, constants['JAN_UNIT_TYPE_PERCENT'])

        print('Error with unit: ' + value)
        exit(1)

    def parseOffset(value):
        items = value.split(' ')
        if len(items) == 1:
            items.append(items[0])
            items.append(items[0])
            items.append(items[0])
        if len(items) == 2:
            items.append(items[0])
            items.append(items[1])
        if len(items) == 3:
            items.append(items[1])
        if len(items) > 4:
            print('Error with offset: ' + value)
            exit(1)
        return [ parseUnit(items[0]), parseUnit(items[1]), parseUnit(items[2]), parseUnit(items[3]) ]

    def parseColor(value):
        value = value.strip()
        red = int(value[1:3], 16)
        green = int(value[3:5], 16)
        blue = int(value[5:], 16)
        return (0xff << 24) | (red << 16) | (green << 8) | blue

    def parseBoolean(value):
        value = value.strip()
        if value == 'false':
            return 0
        if value == 'true':
            return 1
        print('Can\'t parse boolean value: ' + value)
        exit(1)

    def parseFontWeight(value):
        value = value.strip()
        if value == 'normal':
            return constants['JAN_FONT_WEIGHT_NORMAL']
        if value == 'bold':
            return constants['JAN_FONT_WEIGHT_BOLD']
        return int(value)

    def parseOrientation(value):
        value = value.strip()
        if value == 'horizontal':
            return constants['JAN_ORIENTATION_HORIZONTAL']
        if value == 'vertical':
            return constants['JAN_ORIENTATION_VERTICAL']
        print('Can\'t parse orientation value: ' + value)
        exit(1)

    def parseAlign(value):
        align = 0
        for item in value.split('|'):
            item = item.strip()
            if item == 'left':
                align |= constants['JAN_ALIGN_HORIZONTAL_LEFT']
            if item == 'centerHorizontal':
                align |= constants['JAN_ALIGN_HORIZONTAL_CENTER']
            if item == 'right':
                align |= constants['JAN_ALIGN_HORIZONTAL_RIGHT']
            if item == 'top':
                align |= constants['JAN_ALIGN_VERTICAL_TOP']
            if item == 'centerVertical':
                align |= constants['JAN_ALIGN_VERTICAL_CENTER']
            if item == 'bottom':
                align |= constants['JAN_ALIGN_VERTICAL_BOTTOM']
            if item == 'center':
                align |= constants['JAN_ALIGN_HORIZONTAL_CENTER'] | constants['JAN_ALIGN_VERTICAL_CENTER']
        return align

    def dumpWidget(widget):
        if widget.tag == 'Widget':
            file.write(struct.pack('<H', constants['JAN_TYPE_WIDGET']))
        if widget.tag == 'Stack':
            file.write(struct.pack('<H', constants['JAN_TYPE_STACK']))
        if widget.tag == 'Box':
            file.write(struct.pack('<H', constants['JAN_TYPE_BOX']))
        if widget.tag == 'Label':
            file.write(struct.pack('<H', constants['JAN_TYPE_LABEL']))
        if widget.tag == 'Button':
            file.write(struct.pack('<H', constants['JAN_TYPE_BUTTON']))

        file.write(struct.pack('<H', (widget.tag == 'Stack' or widget.tag == 'Box') and len(widget.attrib) + 1 or len(widget.attrib)))
        for attribute in widget.attrib:
            value = widget.attrib[attribute]

            # Widget attributes
            if attribute == 'id':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_ID']))
                file.write(struct.pack('<H', int(value)))

            if attribute == 'width':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_WIDTH']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'height':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_HEIGHT']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'backgroundColor':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_BACKGROUND_COLOR']))
                file.write(struct.pack('<I', parseColor(value)))

            if attribute == 'visible':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_VISIBLE']))
                file.write(struct.pack('B', parseBoolean(value)))

            if attribute == 'margin':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_MARGIN']))
                units = parseOffset(value)
                file.write(struct.pack('f', units[0][0]))
                file.write(struct.pack('B', units[0][1]))
                file.write(struct.pack('f', units[1][0]))
                file.write(struct.pack('B', units[1][1]))
                file.write(struct.pack('f', units[2][0]))
                file.write(struct.pack('B', units[2][1]))
                file.write(struct.pack('f', units[3][0]))
                file.write(struct.pack('B', units[3][1]))

            if attribute == 'marginTop':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_MARGIN_TOP']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'marginLeft':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_MARGIN_LEFT']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'marginRight':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_MARGIN_RIGHT']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'marginBottom':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_MARGIN_BOTTOM']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'padding':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_PADDING']))
                units = parseOffset(value)
                file.write(struct.pack('f', units[0][0]))
                file.write(struct.pack('B', units[0][1]))
                file.write(struct.pack('f', units[1][0]))
                file.write(struct.pack('B', units[1][1]))
                file.write(struct.pack('f', units[2][0]))
                file.write(struct.pack('B', units[2][1]))
                file.write(struct.pack('f', units[3][0]))
                file.write(struct.pack('B', units[3][1]))

            if attribute == 'paddingTop':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_PADDING_TOP']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'paddingLeft':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_PADDING_LEFT']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'paddingRight':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_PADDING_RIGHT']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            if attribute == 'paddingBottom':
                file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_PADDING_BOTTOM']))
                unit = parseUnit(value)
                file.write(struct.pack('f', unit[0]))
                file.write(struct.pack('B', unit[1]))

            # Stack attributes
            if widget.tag == 'Stack':
                if attribute == 'align':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_ALIGN']))
                    file.write(struct.pack('B', parseAlign(value)))

            # Box attributes
            if widget.tag == 'Box':
                if attribute == 'orientation':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_ORIENTATION']))
                    file.write(struct.pack('B', parseOrientation(value)))
                if attribute == 'align':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_ALIGN']))
                    file.write(struct.pack('B', parseAlign(value)))

            # Label & Button attributes
            if widget.tag == 'Label' or widget.tag == 'Button':
                if attribute == 'text':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_TEXT']))
                    file.write(struct.pack('<H', len(value)))
                    file.write(value.encode('UTF-16LE'))
                    file.write(struct.pack('<H', 0))
                if attribute == 'fontName':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_FONT_NAME']))
                    file.write(struct.pack('<H', len(value)))
                    file.write(value.encode('UTF-16LE'))
                    file.write(struct.pack('<H', 0))
                if attribute == 'fontWeight':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_FONT_WEIGHT']))
                    file.write(struct.pack('<H', parseFontWeight(value)))
                if attribute == 'fontItalic':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_FONT_ITALIC']))
                    file.write(struct.pack('B', parseBoolean(value)))
                if attribute == 'fontUnderline':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_FONT_UNDERLINE']))
                    file.write(struct.pack('B', parseBoolean(value)))
                if attribute == 'fontLineThrough':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_FONT_LINE_THROUGH']))
                    file.write(struct.pack('B', parseBoolean(value)))
                if attribute == 'fontSize':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_FONT_SIZE']))
                    unit = parseUnit(value)
                    file.write(struct.pack('f', unit[0]))
                    file.write(struct.pack('B', unit[1]))
                if attribute == 'textColor':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_TEXT_COLOR']))
                    file.write(struct.pack('<I', parseColor(value)))
                if attribute == 'singleLine':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_SINGLE_LINE']))
                    file.write(struct.pack('B', parseBoolean(value)))
                if attribute == 'align':
                    file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_ALIGN']))
                    file.write(struct.pack('B', parseAlign(value)))

        if widget.tag == 'Stack' or widget.tag == 'Box':
            file.write(struct.pack('<H', constants['JAN_ATTRIBUTE_WIDGETS']))
            file.write(struct.pack('<H', len(widget)))
            for widget in widget:
                dumpWidget(widget)

    dumpWidget(root)
