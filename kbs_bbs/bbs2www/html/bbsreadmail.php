<?php
	/**
	 * This file read mails from the mail path file and start index .
	 * by -binxun
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
		if(!strcmp($currentuser["userid"],"guest"))
			html_error_quit("guest û���Լ�������!");
		
		if (isset($_GET["path"])){
			$mail_path = $_GET["path"];
			$mail_title = $_GET["title"];
		}
		else {
			$mail_path = ".DIR";    //default is .DIR
			$mail_title = "�ռ���";
		}
		if (isset($_GET["start"]))
			$start = $_GET["start"];
		else
			$start = 999999;   //default*/
                if (strstr($mail_path,'..'))
			html_error_quit("��ȡ�ʼ�����ʧ��!");
		$mail_fullpath = bbs_setmailfile($currentuser["userid"],$mail_path);
		$mail_num = bbs_getmailnum2($mail_fullpath);
		if($mail_num < 0 || $mail_num > 30000)http_error_quit("Too many mails!");

		$num = 19;
		if ($start > $mail_num - 19)$start = $mail_num - 19;
        if ($start < 0)
		{
		    $start = 0;
			$num = $mail_num;
		}
		$maildata = bbs_getmails($mail_fullpath,$start,$num);
		if ($maildata == FALSE)
			html_error_quit("��ȡ�ʼ�����ʧ��!");

?>
<center>�ż��б� - ���� - <?php echo $mail_title;?>[ʹ����: <?php echo $currentuser["userid"] ?>]<hr color=green>
<?php
	if($mail_num == 0)
	{
		echo "������û���κ��ż�!";
	}
	else
	{
?>
<table width="610">
<tr><td>���<td>״̬<td>������<td>����<td>�ż�����</tr></td>
<?php
		for ($i = 0; $i < $num; $i++)
		{
			/*$ptr = strtok($maildata[$i]["OWNER"]," (");
			if($ptr == 0)$ptr = " ";
			$ptr = nohtml($*/

?>
<tr><td><?php echo $start+$i+1;?><td><?php echo $maildata[$i]["FLAGS"];?><td>
<a href=/bbsqry.php?userid=<?php echo $maildata[$i]["OWNER"];?>><?php echo $maildata[$i]["OWNER"];?></a>
<td><?php echo strftime("%b&nbsp;%e&nbsp;%H&nbsp;:%M",$maildata[$i]["POSTTIME"]);?>
<td><a href=/bbsmailcon.php?dir=<?php echo $mail_path;?>&num=<?php echo $i+$start;?>&title=<?php echo $mail_title;?>><?php
if(strncmp($maildata[$i]["TITLE"],"Re: ",4))
	echo "��" .  htmlspecialchars($maildata[$i]["TITLE"]);
else
    echo htmlspecialchars($maildata[$i]["TITLE"]);
?></a>
</tr>
<?php
		}
	}
?>
</table>
<hr class="default"/>
[�ż�����: <?php echo $mail_num;?>]
[���ÿռ�: <?php echo bbs_getmailusedspace();?>K]
[<a href=/bbspstmail.php>�����ż�</a>]
<?php
		if ($start > 0)
		{
			$i = $start - 19;
			if ($i < 0)$i = 0;
			echo "[<a href=bbsreadmail.php?path=$mail_path&start=0&title=$mail_title>��һҳ</a>] ";
			echo "[<a href=bbsreadmail.php?path=$mail_path&start=$i&title=$mail_title>��һҳ</a>] ";
		}
		if ($start < $mail_num - 19)
		{
			$i = $start + 19;
			if ($i > $mail_num -1)$i = $mail_num -1;
			echo "<a href=bbsreadmail.php?path=$mail_path&start=$i&title=$mail_title>��һҳ</a> ";
			echo "<a href=bbsreadmail.php?path=$mail_path&title=$mail_title>���һҳ</a> ";
		}
?>
<form><input type=submit value=��ת��><input type=hidden name=path value=<?php echo $mail_path;?>>
<input type=hidden name=title value=<?php echo $mail_title;?>>
 �� <input sytle='height:20px' type=text name=start size=3> ��</form>
<?php
	}
?>
