TEMPLATE = subdirs

SUBDIRS += src
SUBDIRS += example

example.depends = src

OTHER_FILES += \
    build.properties \
    build.xml \
    qompoter.json \
    qompoter.pri \
    README.md \
    changelogs.md \

