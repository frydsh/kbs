<?php
	/* this file delete user mails  windinsn nov 15,2003*/
	require("funcs.php");
	
	if ($loginok != 1)
		html_nologin();
	elseif(!strcmp($currentuser["userid"],"guest"))
	{
		html_init("gb2312");
		html_error_quit("guest û���Լ�������!");
		die();
	}
	else
	{
		html_init("gb2312","","",1);
		$dirname = $_POST["dir"];
		$title = $_POST["title"];
		$dstart = $_POST["dstart"];
		$dend = $_POST["dend"];
		$dtype = $_POST["dtype"];
		
		if (strstr($dirname,'..'))
		{
			html_error_quit("��ȡ�ʼ�����ʧ��!");
                        die();
                }
		
		$mail_fullpath = bbs_setmailfile($currentuser["userid"],$dirname);
		$mail_num = bbs_getmailnum2($mail_fullpath);
			
		if( $dstart < 1 || $dstart > $mail_num  || $dend < 1 || $dend > $mail_num  || $dstart > $dend  )
		{
			html_error_quit("����ɾ����ʼ����������������������룡");
			die();
		}
		
?>
<p align="left" class="b2">
<a href="bbssec.php" class="b2"><?php echo BBS_FULL_NAME; ?></a>
-
<a href="bbsmail.php">
<?php echo $currentuser["userid"]; ?>������
</a>
-
����ɾ���ʼ�
</p>
<p align="left">
�������� <font class="b3"><?php echo $title; ?></font> �н�������ɾ���ʼ���<br>
��ʼ�ʼ���ţ�<font class="b3"><?php echo $dstart; ?></font><br>
�����ʼ���ţ�<font class="b3"><?php echo $dend; ?></font><br>
ɾ�����ͣ�<font class="b3">
<?php 
	if($dtype == 1)
		echo "ǿ��ɾ��";
	else
		echo "��ͨɾ��";
?>
</font><br>
</p>
<form action="/bbsmailact.php?act=move&<?php echo "dir=".urlencode($dirname)."&title=".urlencode($title); ?>" method="POST">
<input type="hidden" name="act2" value="delarea">
<?php			
		$dnum = $dend - $dstart + 1;
		$dstart-- ;
		$j = 0;
		
		$maildata = bbs_getmails($mail_fullpath,$dstart,$dnum);
		if($dtype == 1)
		{
			foreach( $maildata as $mail )
			{
				echo "<input type=\"hidden\" name=\"file".$j."\" value=\"".$mail["FILENAME"]."\">\n";
				$j ++;
			}
		}
		else
		{
			foreach( $maildata as $mail )
			{
				if(stristr($mail["FLAGS"],"m"))
					continue;
				else
				{
					echo "<input type=\"hidden\" name=\"file".$j."\" value=\"".$mail["FILENAME"]."\">\n";
					$j ++;
				}
			}
		}
?>
<input type="hidden" name="mailnum" value="<?php echo $j; ?>">
<input type="button" value="�޸�" class="bt1" onclick="history.go(-1)">
<input type="submit" value="ȷ��" class="bt1">
</form>
<?php	
	}
	
?>