TEMPLATE = subdirs

SUBDIRS += MapGraphics \
           UAV

UAV.depends += MapGraphics
