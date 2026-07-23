#coding=utf-8

from page.manager import myweb

import sys
reload(sys)
sys.setdefaultencoding('utf8')


if myweb.init(): exit()
print "系统初始化完成"

#myweb.work()               #调试
application = myweb.app.wsgifunc()
