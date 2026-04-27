#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, zipfile, re, html
from pathlib import Path
import xml.etree.ElementTree as ET

def extract(path: Path) -> str:
    ns={'w':'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
    with zipfile.ZipFile(path) as z:
        xml=z.read('word/document.xml')
    root=ET.fromstring(xml)
    paras=[]
    for p in root.findall('.//w:p', ns):
        texts=[]
        for t in p.findall('.//w:t', ns):
            texts.append(t.text or '')
        if texts:
            paras.append(''.join(texts))
    return '\n'.join(paras)

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('docx')
    ap.add_argument('--out')
    args=ap.parse_args()
    text=extract(Path(args.docx))
    if args.out:
        Path(args.out).write_text(text, encoding='utf-8')
    else:
        print(text)
if __name__=='__main__': main()
