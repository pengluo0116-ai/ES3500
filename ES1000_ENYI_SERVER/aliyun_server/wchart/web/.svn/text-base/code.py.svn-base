#coding=utf-8
# 接收微信事件推送
import web
import time
import MySQLdb 
import json
import redis
import WeChat
import sys
reload(sys)
sys.setdefaultencoding('utf8')

urls = (
	'/', 'Hello'
)

r = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)
app = web.application(urls, globals())
application = app.wsgifunc()


class Hello:
	def POST(self):
		sn_legal = False		# sn序列号合法性标识，默认为False
		recv = web.data()		# 接收报警主机发送来的数据
		# 尝试解析数据，判断数据合法性
		try:
			hjson = json.loads(recv)
			sn = hjson.get('SN')
			wx_info_list = hjson.get('wx_info')
			alarm_info = hjson.get('alarm_info')
		except:
			print "Data format is wrong!"

		# 读取数据库中的序列号，并判断序列号的合法性
		try:
			conn = MySQLdb.connect(
				host = 'localhost',
				port = 3306,
				user = 'root',
				passwd = 'sdne_7572',
				db = 'enyi_lift',
			)
			cur = conn.cursor()
			cnt = cur.execute("select * from sn_emmc")	# 返回表中的数据条数
			info = cur.fetchmany(cnt)					# 获取表中的所有序列号（sn）
			for cnt in info:
				if cnt[1] == sn:
					sn_legal = True
					print "SN is legal"
					break
				else:
					continue
			cur.close()
			conn.commit()
			conn.close()
		except:
			print 'Database error!'

		# 如果序列号合法，生成模板报警信息并推送
		if sn_legal:
			fault_msg = ""			# 故障信息
			alarm_info = json.loads(alarm_info)

			Parameters = alarm_info.get('Parameters')
			dev_name = Parameters.get("dev_name")
			alarm_time = Parameters.get("Recv_time")

			Alarm = alarm_info.get("Alarm")
			if Alarm.get('Kaceng') == "1":
				fault_msg += "卡层"

			if Alarm.get('Kunren') == "1":
				if fault_msg == "":
					fault_msg += "困人"
				else:
					fault_msg += "，困人"

			if Alarm.get('UP_Alarm') == "1":
				if fault_msg == "":
					fault_msg += "冲顶"
				else:
					fault_msg += "，冲顶"

			if Alarm.get('Down_Alarm') == "1":
				if fault_msg == "":
					fault_msg += "蹲底"
				else:
					fault_msg += "，蹲底"

			if Alarm.get('Run_door') == "1":
				if fault_msg == "":
					fault_msg += "开门走梯"
				else:
					fault_msg += "，开门走梯"

			if Alarm.get('Speeding') == "1":
				if fault_msg == "":
					fault_msg += "电梯超速"
				else:
					fault_msg += "，电梯超速"

			if Alarm.get('Power') == "1":
				if fault_msg == "":
					fault_msg += "电梯掉电"
				else:
					fault_msg += "，电梯掉电"
			
			postBody = {}
			postBody["first"] = {"value": "电梯报警", "color": "#FF0000"}  # 标题
			postBody["keyword1"] = {"value": "系统报警", "color": "#000000"}  # 故障用户
			postBody["keyword2"] = {"value": alarm_time, "color": "#000000"}  # 故障时间
			postBody["keyword3"] = {"value": fault_msg, "color": "#000000"}  # 故障现象
			# postBody["remark"] = {"value": "地址：山东省济南市高新开发区\n上次维保日期：2015-10-15\n测试发送", "color": "#173177"}  # 故障现象
			postBody["remark"] = {"value": "电梯地址：" + dev_name, "color": "#000000"}	# 故障电梯名
			tmp = WeChat.server()
			access_token = r.get("wtoken")			# 获取access_token
			for i in wx_info_list:
				err = tmp.sendModelMsg(
					i["appid"],
					access_token,
					"3gFJjNbu73C23pqQ0Aa2JPtLbQ2cwaiccv_EhihXCOs",
					"",
					postBody
				)	# ""表示接收的消息无法点进去看详情
				print str(err)
		




if __name__=='__main__':
	#app = web.application(urls, globals())
	app.run()
