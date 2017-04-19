# LogCatTool
Android LogCatTool 
#软件简介
Android常用开发工具Eclipse和Android Studio本身自带有日志查看工具LogCat，一般性使用基本满足要求。但若长期处于Android的深度开发，会发现自带的工具内容缓冲区有限，会造成历史数据的丢失，且无法进行信息过滤和日志头自动提取分类。为了解决这些问题，故而开发了此款软件，该软件不仅解决了上述问题，而且还支持对日志文件进行离线分析和导出备份，支持对日志内容的横向过滤和纵向过滤，且可通过ADB工具直连物理设备进行日志的监控和分析，无需依赖开发工具。另外，软件为完全自主开发，对后期若有新的功能需求可以很方便的进行功能扩展和维护。
欢迎反馈意见到964195140@qq.com


软件功能简介
左侧Filters为日志头过滤器栏，当载入日志文件时会自动识别；
日志内容会自动根据打印等级显示不同的颜色以区分；
可通过设置右上角的日志等级筛选出大于等于该等级的打印信息；
可通过中上方的搜索框搜索日志内容，搜索规则为：任意字符串，大小写不敏感；
可通过设置时间过滤器的起止时间来筛选出在该时间范围内的所有日志信息；
可通过设置信息过滤器的开关来选择是否显示对应的信息；
可通过工具栏中模式切换按钮在“离线查看模式”和“实时终端模式”之间切换；
可通过工具栏中ADB连接按钮，进行ADB设备的扫描、连接、断开等操作；
软件底部显示当前工作的状态，如：工作模式、ADB设备状态、当前打开的日志文件路径;
退出实时终端模式时可将日志缓存文件导出另存；可在软件设置中配置外部ADB工具的路径；
对日志内容解析后，可提供：行号、时间、等级、PID进程号、Tag、文本信息；
在实时终端模式中，ADB设备断开或丢失后可自动重连3次，并自动恢复日志内容的显示，若3次重连失败则退出ADB环境；
可通过将鼠标移动到Tag列表栏的某一项上，预览该Tag的完整信息；文件加载过程有进度条提示；
可通过菜单栏中最近打开的文件选项快速打开文件；
可通过快捷键“Ctrl+G”或者工具栏中的对应图标快速跳转到指定行；
支持保存软件的全局配置属性和重要数据到配置文件中，待软件下次启动时可恢复上一次的操作环境；若该文件不存在，则会自动生成配置文件;
【注意】实时终端模式下，每次只能连续实时监控1小时，超过该时间后由于日志文件过大，导致解析时间比较长，无法达到实时显示的效果；但后台仍然在将日志内容输出到缓存文件，可等待执行完后再将该文件导出后进行离线分析即可;
