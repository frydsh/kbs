<?php
	require("funcs.php");

	html_init("gb2312");

if($loginok != 1)
	html_no_login();
else{

	if (isset($_GET["userid"])){
		$userid = $_GET["userid"];

		$lookupuser=array ();
		if( $userid=="" || bbs_getuser($userid, $lookupuser) == 0 )
			html_error_quit("���û�������");

		$usermodestr = bbs_getusermode($userid);
		// ֮���Բ�ʹ�� strrpos() ����Ϊ $lookupuser["lasthost"]
		// �ĳ��Ȳ���
		$str = $lookupuser["lasthost"];
		$len = strlen($str);
		for ($i = $len - 1; $i >= 0; --$i)
		{
			if ($str[$i] == ".")
				break;
		}
		if ($i < 0)
			$lasthost_enc = "(δ֪)";
		else
			$lasthost_enc = substr_replace($lookupuser["lasthost"], "*", $i + 1);
?>
<center><?php echo BBS_FULL_NAME; ?> -- ��ѯ����<hr color=green>
</center><pre>
<?php echo $lookupuser["userid"];?> (<?php echo $lookupuser["username"];?>) ����վ <?php echo $lookupuser["numlogins"];?> �Σ������ <?php echo $lookupuser["numposts"];?> ƪ����
�ϴ���  [<?php echo date("D M j H:i:s Y",$lookupuser["lastlogin"]);?>] �� [<?php echo $lasthost_enc;?>] ����վһ�Ρ�
����ʱ��[<?php 

if( $usermodestr!="" && $usermodestr{0}=="1" ){
	echo date("D M j H:i:s Y", $lookupuser["lastlogin"]+60+( $lookupuser["numlogins"]+$lookupuser["numposts"] )%100 );
} else if( $lookupuser["exittime"] < $lookupuser["lastlogin"] )
	echo "�������ϻ�ǳ����߲���";
else
	echo date("D M j H:i:s Y", $lookupuser["exittime"]); 

?>] ���䣺[<?php if( bbs_checknewmail($lookupuser["userid"]) ) echo "��"; else echo "  ";?>] ��������[<?php echo bbs_compute_user_value($lookupuser["userid"]); ?>] ���: [<?php echo bbs_user_level_char($lookupuser["userid"]); ?>]��
<?php if( $usermodestr!="" && $usermodestr{1} != "") echo substr($usermodestr, 1); ?>
</pre>
<?php
		$plansfile = "home/".strtoupper($lookupuser["userid"]{0})."/".$lookupuser["userid"]."/plans";

		if( file_exists( $plansfile ) ){
?>
<font class="c36">����˵�������£�<font class="c37">
<br>
<?php
			echo bbs_printansifile("home/".strtoupper($lookupuser["userid"]{0})."/".$lookupuser["userid"]."/plans");
		}else{
?>
<font class="c36">û�и���˵����<font class="c37">
<?php
		}
?>
<br><br>
<center>
<a href="/bbspstmail.php?userid=<?php echo $lookupuser["userid"];?>&title=û����">[д���ʺ�]</a>
<a href="/bbssendmsg.php?destid=<?php echo $lookupuser["userid"];?>">[����ѶϢ]</a>
<a href="/bbsfadd.php?userid=<?php echo $lookupuser["userid"];?>">[�������]</a>
<a href="/bbsfdel.php?userid=<?php echo $lookupuser["userid"];?>">[ɾ������]</a>
<a href="/pc/index.php?id=<?php echo $lookupuser["userid"];?>">[�����ļ�]</a>
<br>
</center>
<?php
		html_normal_quit();
?>
<?php
	}else{
?>

<center><?php echo BBS_FULL_NAME; ?> -- ��ѯ����<hr color=green>
<form action=/bbsqry.php method=get>
�������û���: <input name=userid maxlength=12 size=12>
<input type=submit value=��ѯ�û�>
</form><hr>
<?php
		html_normal_quit();
	}
}
?>
