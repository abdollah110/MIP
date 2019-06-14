#! /usr/bin/python

# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__ = "fahim"
__date__ = "$Nov 25, 2010 1:29:30 PM$"

from sys import argv
import sys
import os

if __name__ == "__main__":
    pdf = sys.argv[1]
    pngs = sys.argv[2:]

    for i in pngs:
        command = os.popen("convert " + i + " " + i + ".pdf")

    command = "gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=" + pdf
    for i in pngs:
        command = command + " " + i + ".pdf"

    command = os.popen(command)

    command = "rm -rf "
    for i in pngs:
        command = command + i + ".pdf "

    command = os.popen(command)




