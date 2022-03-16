# upup
![IMAGE](https://raw.githubusercontent.com/krzysckh/upup/master/upup.png)

upup is a tiny and minimal markup language.


# how to install

```
git clone https://github.com/krzysckh/upup
cd upup
make
sudo make install
```


# how to use

please, take a look at ```upup(1)``` and examples

to easily convert *upup*  to pdf, use
```
upup doc.uu -Tmom | preconv -e utf-8 | groff -mom -m www -Tps | ps2pdf - doc.pdf
```
it will use **curl(1)**  and **convert(1)**  to download images from the internet and put them 
into the document
