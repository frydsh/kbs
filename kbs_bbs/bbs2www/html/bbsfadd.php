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
<center><?php echo BBS_FULL_NAME; ?> -- ���Ӻ��� [ʹ����: <?php echo $currentuser["userid"]; ?>]<hr color="green">
<?php
		if (isset($_GET["userid"]))
		{
			$duserid = $_GET["userid"];
			if( isset($_GET["exp"]))
				$expp = $_GET["exp"];
			else
				$expp="";
			$ret = bbs_add_friend( $duserid ,$expp );
			if($ret == -1){
				html_error_quit("��û��Ȩ���趨���ѻ��ߺ��Ѹ�����������");
			}else if($ret == -2){
				html_error_quit("���˱���������ĺ���������");
			}else if($ret == -3){
				html_error_quit("ϵͳ����");
			}else if($ret == -4){
				html_error_quit("�û�������");
			}else{
?>
[<?php echo $duserid; ?>]�����ӵ����ĺ���������.<br>
<a href="/bbsfall.php">���غ�������</a>
<?php
				html_normal_quit();
			}
		}else{
?>
<form action="/bbsfadd.php">
�����������ӵĺ����ʺ�:<input maxlength=12 name="userid" type="text"><br>
�����������ӵĺ��ѱ�ע:<input maxlength=14 name="exp" type="text"><br>
<input type="submit" value="ȷ��">
</form>
<a href="/bbsfall.php">���غ�������</a>
<?php
			html_normal_quit();
		}
	}
?>
