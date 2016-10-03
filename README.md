# HGT2JSON-OSX

## What is this?

This is a data generator program. This generate JSON format altitude data from HGT file.
This program can be run only OSX.

## Description

This program generate JSON format altitude data which can be accessed by javascript. Altitude data is converted from hgt binary data.
About hgt file or other type of elevation data, visit web site as follows.

>  [Global Elevation Datasets]: http://vterrain.org/Elevation/global.html
>  [Jonathan de Ferranti's Digital Elevation Data site]: http://www.viewfinderpanoramas.org/dem3.html

## How to build

1. If you have not installed boost yet, try to install it first.
   I believe `brew install boost` is the best way.
2. Open `hgt2json-osx.xcodeproj` file, then xcode appears.
3. Change the include and library path to boost.
   You can check the install folder of boost by using `brew info boost` command.
4. Just build!       

## How to use
`hgt2json-osx -D path/to/hgt/ -J path/to/output/ -S 1 -C 400`

    *options
        -D : hgt file's folder
        -J : output folder
        -S : If you want to make altitude data smooth by gaussian filter, set this to '1'
        -C : This parameter make altitude data smooth by discrete cosign transform.  

## LICENSE

This program is MIT LICENSE
See LICENSE file.
