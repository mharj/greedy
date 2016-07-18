# greedy
multithread metadata scanner

Collects per uid/gid metadata (files,dirs,total size) and yearly modified data age (size) list in json format

Build:
```bash
qmake && make
```

```bash
$ ./greedy.x86_64 /usr
```
```json
{
  "uids": [
    {
      "uid": 0,
      "files": 150652,
      "dirs": 15229,
      "size": 3622075788,
      "mtime": 1468823943,
      "ctime": 1468823943,
      "atime": 1468836976
    }
  ],
  "gids": [
    {
      "gid": 0,
      "files": 150641,
      "dirs": 15209,
      "size": 3620931812,
      "mtime": 1468823943,
      "ctime": 1468823943,
      "atime": 1468836976
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
      "atime": 1468822312
    },
    {
      "gid": 50,
      "files": 0,
      "dirs": 19,
      "size": 77824,
      "mtime": 1468570227,
      "ctime": 1468570227,
      "atime": 1468836965
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
      "atime": 1468822306
    },
    {
      "gid": 113,
      "files": 0,
      "dirs": 1,
      "size": 4096,
      "mtime": 1458927689,
      "ctime": 1468570232,
      "atime": 1468836960
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
      "atime": 1468822299
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
    "0": 35576,
    "1": 4321,
    "2": 49,
    "3": 1556,
    "4": 2095,
    "5": 1100,
    "6": 1048,
    "7": 3562,
    "8": 5042,
    "9": 8184,
    "10": 963,
    "11": 59823,
    "12": 8642,
    "13": 8418,
    "14": 3032,
    "15": 203338,
    "16": 8347,
    "17": 1445,
    "18": 1764,
    "19": 3975,
    "20": 263,
    "21": 3960
  },
  "stats": {
    "elapsed": 1020,
    "fps": 162629
  }
}
```



