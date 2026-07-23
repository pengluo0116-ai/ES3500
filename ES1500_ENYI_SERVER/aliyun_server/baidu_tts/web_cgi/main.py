#coding:utf-8


from page.manager import myweb
from cont.cont_baiduTts import cont_baiduTts

import sys
reload(sys)
sys.setdefaultencoding('utf8')


if cont_baiduTts.check_init(): exit()
print "系统初始化完成"

print myweb.init()
application = myweb.app.wsgifunc()

