<?php
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
        if ($currentuser["userid"]=="guest")
            html_error_quit("�Ҵҹ��Ͳ����޸��ǳ�");
?>
<body>
<center><?php echo BBS_FULL_NAME; ?> -- ��ʱ�ı��ǳ�(�����ķ���Ч) [ʹ����: <?php echo $currentuser["userid"]; ?>]<hr color="green">
<?php
		if (isset($_GET["submit"]) || isset($_POST["submit"]))
		{
			if( isset($_POST["username"]) ){
				$username = $_POST["username"];
				if( strlen($username) < 2 )
					html_error_quit("�ǳ�̫��");
			}else
				html_error_quit("�ǳƱ�����д");

			$ret = bbs_modify_nick( $username );

			if($ret < 0){
				html_error_quit("ϵͳ����");
			}else{
?>
<?php echo $currentuser["userid"]; ?>��ʱ�ǳ��޸ĳɹ�<br>
<?php
				html_normal_quit();
			}
		}else{
?>
<form action="/bbsnick.php" method=post>
���ǳ�: <input type="text" name="username" value="<?php echo $currentuser["username"];?>" size=24 maxlength=39><br>
<input type="submit" name="submit" value="ȷ��">
</form>
<hr>
<?php
			html_normal_quit();
		}
	}
?>
