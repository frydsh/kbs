<?php
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
        if ($currentuser["userid"]=="guest")
            html_error_quit("�Ҵҹ��Ͳ����趨����");
?>
<body>
<center><?php echo BBS_FULL_NAME; ?> -- ɾ������ [ʹ����: <?php echo $currentuser["userid"]; ?>]<hr color="green">
<?php
		if (isset($_GET["userid"]))
		{
			$duserid = $_GET["userid"];
			$ret = bbs_delete_friend( $duserid );
			if($ret == 1){
				html_error_quit("��û���趨�κκ���");
			}else if($ret == 2){
				html_error_quit("���˱����Ͳ�����ĺ���������");
			}else if($ret == 3){
				html_error_quit("ɾ��ʧ��");
			}else{
?>
[<?php echo $duserid; ?>]�Ѵ����ĺ���������ɾ��.<br>
<a href="/bbsfall.php">���غ�������</a>
<?php
				html_normal_quit();
			}
		}else{
?>
<form action="/bbsfdel.php">
��������ɾ���ĺ����ʺ�:<input name="userid" type="text"><br>
<input type="submit" value="ȷ��">
</form>
<a href="/bbsfall.php">���غ�������</a>
<?php
			html_normal_quit();
		}
	}
?>
