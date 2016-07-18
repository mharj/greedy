# greedy
metadata scanner

Collects per uid/gid metadata (files,dirs,total size) and yearly data age (size) list in json format
```json
{
  "uids": [
    {
      "uid": 0,
      "files": 161,
      "dirs": 0,
      "size": 13093546,
      "mtime": 1468569705,
      "ctime": 1468578131,
      "atime": 1468829899
    }
  ],
  "gids": [
    {
      "gid": 0,
      "files": 161,
      "dirs": 0,
      "size": 13093546,
      "mtime": 1468569705,
      "ctime": 1468578131,
      "atime": 1468829899
    }
  ],
  "year": {
    "0": 60680,
    "1": 98240,
    "2": 44168,
    "3": 31248
  },
  "stats": {
    "elapsed": 2,
    "fps": 81000
  }
}
```


Build:
```bash
qmake && make
```
