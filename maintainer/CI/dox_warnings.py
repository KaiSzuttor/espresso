#!/usr/bin/env python3
#
# Copyright (C) 2019 The ESPResSo project
#
# This file is part of ESPResSo.
#
# ESPResSo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ESPResSo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
import re
import os


def read_warnings():
    """Read Doxygen warnings and add extra warnings about ``@param varname``
    missing a description"""

    # collect list of Doxygen warnings
    with open('doc/doxygen/warnings.log') as f:
        content = f.read()

    raw_warnings = re.findall(
        r'(?:^|\n)doxygen:(.+?):(\d+): warning: (.+?)(?=\n\S|\n*$)',
         content, re.DOTALL)

    # collect list of empty @param and @tparam blocks
    with open('doc/doxygen/empty-params.log') as f:
        content = f.read().strip()

    if content:
        source_code_ext = set(['.hpp', '.cpp', '.hh', '.cc', '.h', '.c',
                               '.cuh', '.cu', '.dox'])
        for line in content.strip().split('\n'):
            m = re.search(r'^(.+):(\d+):[\s\*]*([@\\]t?param).*\s(\S+)\s*$',
                          line)
            filepath, lineno, paramtype, varname = m.groups()
            ext = os.path.splitext(filepath)[1]
            if ext.lower() not in source_code_ext:
                continue
            warning = ('argument \'{0}\' of {1} has no description, either add'
                       ' one or remove {1}'.format(varname, paramtype))
            raw_warnings.append((filepath, lineno, warning))

    # remove duplicated warnings
    n_all = len(raw_warnings)
    raw_warnings = {(filepath, int(lineno), warning.split('\n')[0]):
                    set(warning.split('\n')[1:])
                    for filepath, lineno, warning in raw_warnings}
    n_unique_raw = len(raw_warnings)
    return raw_warnings, n_all, n_unique_raw


def read_classes_with_warnings():
    """Collect documentation files of classes where method argument name
    warnings were raised"""
    with open('doc/doxygen/class_methods.log') as f:
        content = f.read()

    classes = {}
    if content.strip():
        classes = ('\n' + content.strip()).split('\nwarning:')[1:]
        classes = {x[0]: x[1:] for x in [x.split('\n') for x in classes]}

    return classes


def is_argument_not_found_false_positive(argument_name, filepath, lineno_warn):
    """Return True if a documented argument is found in a function declaration"""
    if filepath not in classes:
        return False
    for xml_file in classes[filepath]:
        with open(xml_file) as f:
            content = f.read()
        memberdefs = re.findall('<memberdef .+?</memberdef>',
                                content, re.DOTALL)
        membderdefs_lineno = []
        for node in memberdefs:
            m = re.search('<location file="' + re.escape(filepath) +
                          '" line="(\d+)"', node)
            if m:
                membderdefs_lineno.append((int(m.group(1)), node))
        for node_lineno, node in sorted(membderdefs_lineno):
            if node_lineno > lineno_warn:
                break
        else:
            continue
        m = re.search('<argsstring>(.+?)</argsstring>', node, re.DOTALL)
        if not m:
            continue
        if re.search('[^a-zA-Z0-9_]' + re.escape(argument_name) + '\s*(?=,|\)$)', m.group(1)):
            return True
    return False


# read files
classes = read_classes_with_warnings()
raw_warnings, n_all, n_unique_raw = read_warnings()

# filter out non-critical warnings
warnings = {}
for (filepath, lineno, warning), warning_list in raw_warnings.items():
    if re.search('^member \S+ belongs to two different groups\. '
                 'The second one found here will be ignored\.$', warning):
        # happens when a function is declared in a group in the .hpp file but
        # defined in another group in the .cpp file; this is usually caused by
        # the "Private functions" and "Exported functions" groups in .hpp files
        continue
    if re.search('^documented symbol `\S+\' was not declared or defined\.$', warning):
        # known bug, fixed in 1.8.16
        continue
    if re.search('^no uniquely matching class member found for $', warning):
        # known bug, not fixed yet
        continue
    if re.search('^The following parameters of .+ are not documented:$', warning):
        # non-critical warning, enforcing it would encourage bad behavior, i.e.
        # inserting "@param argument" without a description to silence the
        # warning, when in reality the warning is silenced because the text on
        # the following line is captured and becomes the argument description
        continue
    m = re.search(r'^argument \'(\S+)\' of command [@\\]param is not found '
                  r'in the argument list of ', warning)
    if m and is_argument_not_found_false_positive(m.group(1), filepath, lineno):
        # bug in doxygen where arguments of class methods are erased from
        # <declname> nodes if a method with the same name exists in that class
        continue
    filepath = re.sub('^.*(?=src/)', '', filepath)
    if filepath not in warnings:
        warnings[filepath] = {}
    warnings[filepath][(lineno, warning)] = warning_list


n_unique = sum(map(len, warnings.values()))
if n_unique == 0:
    with open('dox_warnings.log', 'w') as f:
        pass
    exit(0)


# generate a log file
with open('dox_warnings.log', 'w') as f:
    f.write('The Doxygen documentation generated {} unique warnings (total: {},'
            ' ignored: {}):\n'.format(n_unique, n_all, n_unique_raw - n_unique))
    for filepath in sorted(warnings.keys()):
        f.write(filepath + ':\n')
        for (lineno, warning) in sorted(warnings[filepath].keys()):
            warning_list = warnings[filepath][(lineno, warning)]
            s = re.sub('\(.*\)', '()', warning)
            if warning_list:
                s += ': ' + ', '.join(x.strip() for x in warning_list)
            f.write('  line {}: {}\n'.format(lineno, s))

exit(1)
