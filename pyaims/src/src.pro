#!include ../config-local

TEMPLATE = subdirs

SUBDIRS  = libpyaims

PSUBDIRS = sip

module(pyaimsgui)  :PSUBDIRS	+= sip_gui

