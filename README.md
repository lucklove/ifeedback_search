#ifeedback_search

[轻反馈](http://www.qingfankui.com/)题目搜索算法  

|    操作    |    方法    |                            地址                        |      BODY                 |     备注     |
|------------|------------|--------------------------------------------------------|---------------------------|--------------|
|  添加题目  |    POST    |  192.168.72.128:1024/set/id?subject=s_id&type=t_id     |  {"content": "题目内容"}  |              |
|  删除题目  |   DELETE   |  192.168.72.128:1024/delete/id                         |                           |              |
|  修改题目  |    POST    |  192.168.72.128:1024/set/id&subject=s_id&type=t_id     | {"content": "修改后内容"} |              |
|  搜索题目  |    POST    |  192.168.72.128:1024/search?subject=s_id&type=t_id&offset=ofst&limit=lmt | {"content": "用户输入"} | subject, type, offset, limit可选 |
