<?php
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
        if ($currentuser["userid"]=="guest")
            html_error_quit("�Ҵҹ��Ͳ����޸�����");
?>
<body>
<center><?php echo BBS_FULL_NAME; ?> -- �û��������� [ʹ����: <?php echo $currentuser["userid"]; ?>]<hr color="green">
<?php
		if (isset($_GET["submit"]) || isset($_POST["submit"]))
		{
			if( isset($_POST["username"]) ){
				$username = $_POST["username"];
				if( strlen($username) < 2 )
					html_error_quit("�ǳ�̫��");
			}else
				html_error_quit("�ǳƱ�����д");

			if( isset($_POST["realname"]) ){
				$realname = $_POST["realname"];
				if( strlen($realname) < 2 )
					html_error_quit("��ʵ����̫��");
			}else
				html_error_quit("��ʵ����������д");

			if( isset($_POST["address"]) ){
				$address = $_POST["address"];
				if( strlen($address) < 8 )
					html_error_quit("��ס��ַ̫��");
			}else
				html_error_quit("��ס��ַ������д");

			if( isset($_POST["email"]) ){
				$email = $_POST["email"];
				if( strlen($email) < 8 )
					html_error_quit("email���Ϸ�");
			}else
				html_error_quit("email������д");

			$ret = bbs_modify_info( $username, $realname, $address, $email );

			if($ret < 0){
				html_error_quit("ϵͳ����");
			}else{
?>
<?php echo $currentuser["userid"]; ?>���������޸ĳɹ�<br>
<?php
				html_normal_quit();
			}
		}else{
?>
<form action="/bbsinfo.php" method=post>
�����ʺ�: <?php echo $currentuser["userid"];?><br>
�����ǳ�: <input type="text" name="username" value="<?php echo $currentuser["username"];?>" size=24 maxlength=39><br>
�������: <?php echo $currentuser["numposts"];?> ƪ<br>
��վ����: <?php echo $currentuser["numlogins"];?> ��<br>
��վʱ��: <?php echo $currentuser["stay"]/60;?> ����<br>
��ʵ����: <input type="text" name="realname" value="<?php echo $currentuser["realname"];?>" size=16 maxlength=39><br>
��ס��ַ: <input type="text" name="address" value="<?php echo $currentuser["address"];?>" size=40 maxlength=79><br>
�ʺŽ���: <?php echo date("D M j H:i:s Y",$currentuser["firstlogin"]);?><br>
�������: <?php echo date("D M j H:i:s Y",$currentuser["lastlogin"]);?><br>
��Դ��ַ: <?php echo $currentuser["lasthost"];?><br>
�����ʼ�: <input type="text" name="email" value="<?php echo $currentuser["email"];?>" size=32 maxlength=79><br>
<input type="submit" name="submit" value="ȷ��"><input type="reset" value="��ԭ">
</form>
<hr>
<?php
			html_normal_quit();
		}
	}
?>
