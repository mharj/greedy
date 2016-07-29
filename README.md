# greedy
multithread metadata scanner

Collects per uid/gid metadata (files,dirs,total size,latest atime/ctime/mtime) and yearly modified data age (size) list in json format

Build:
```bash
qmake && make
```

```bash
$ ./greedy.x86_64 /usr
```
```json
{
  "all": {
    "files": 151032,
    "dirs": 15237,
    "size": 3824173063,
    "mtime": 1469440719,
    "ctime": 1469440719,
    "atime": 1469779955
  },
  "uids": [
    {
      "uid": 0,
      "files": 151032,
      "dirs": 15237,
      "size": 3824173063,
      "mtime": 1469440719,
      "ctime": 1469440719,
      "atime": 1469779955
    }
  ],
  "gids": [
    {
      "gid": 0,
      "files": 151021,
      "dirs": 15217,
      "size": 3823029087,
      "mtime": 1469440719,
      "ctime": 1469440719,
      "atime": 1469779955
    },
    {
      "gid": 5,
      "files": 2,
      "dirs": 0,
      "size": 42120,
      "mtime": 1464305491,
      "ctime": 1468570005,
      "atime": 1468570005
    },
    {
      "gid": 30,
      "files": 1,
      "dirs": 0,
      "size": 390888,
      "mtime": 1454061683,
      "ctime": 1468570170,
      "atime": 1468570037
    },
    {
      "gid": 42,
      "files": 2,
      "dirs": 0,
      "size": 85104,
      "mtime": 1459243509,
      "ctime": 1468569694,
      "atime": 1468569698
    },
    {
      "gid": 43,
      "files": 1,
      "dirs": 0,
      "size": 10232,
      "mtime": 1457700835,
      "ctime": 1468570073,
      "atime": 1469779426
    },
    {
      "gid": 50,
      "files": 0,
      "dirs": 19,
      "size": 77824,
      "mtime": 1468570227,
      "ctime": 1468570227,
      "atime": 1469779928
    },
    {
      "gid": 107,
      "files": 1,
      "dirs": 0,
      "size": 36080,
      "mtime": 1459893553,
      "ctime": 1468569705,
      "atime": 1468569700
    },
    {
      "gid": 110,
      "files": 1,
      "dirs": 0,
      "size": 42992,
      "mtime": 1459528873,
      "ctime": 1468569810,
      "atime": 1469779420
    },
    {
      "gid": 113,
      "files": 0,
      "dirs": 1,
      "size": 4096,
      "mtime": 1458927689,
      "ctime": 1468570232,
      "atime": 1469779924
    },
    {
      "gid": 115,
      "files": 1,
      "dirs": 0,
      "size": 39520,
      "mtime": 1416297283,
      "ctime": 1468570224,
      "atime": 1468570037
    },
    {
      "gid": 116,
      "files": 1,
      "dirs": 0,
      "size": 358624,
      "mtime": 1461833562,
      "ctime": 1468570224,
      "atime": 1469779412
    },
    {
      "gid": 65534,
      "files": 1,
      "dirs": 0,
      "size": 56496,
      "mtime": 1460680118,
      "ctime": 1468570082,
      "atime": 1468570037
    }
  ],
  "year": {
    "0": 3411302016,
    "1": 284207292,
    "2": 50809684,
    "3": 28196978,
    "4": 26706846,
    "5": 7097641,
    "6": 7677419,
    "7": 218056,
    "8": 4676078,
    "9": 1120120,
    "10": 681241,
    "11": 431000,
    "12": 164644,
    "13": 146375,
    "14": 163292,
    "15": 358982,
    "16": 137661,
    "17": 11094,
    "18": 28678,
    "19": 31700,
    "20": 708,
    "21": 5558
  },
  "stats": {
    "elapsed": 851,
    "fps": 195382
  }
}
```



