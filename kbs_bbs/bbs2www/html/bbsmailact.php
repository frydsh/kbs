<?php
	/*this file do some mail's action	$id:windinsn nov 8,2003	*/
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
		$act = $_GET["act"];
		$dirname = $_GET["dir"];
		$title = $_GET["title"];
		$act2 = $_POST["act2"];
		
		if($act == "clear" )
		{
			$dirname = ".DELETED";
			$title = "������";
		}
		
		if(!isset($_POST["mailnum"]) || $_POST["mailnum"] == 0)
			$mail_num = 19;
		else
			$mail_num = $_POST["mailnum"];
		
		if (strstr($dirname,'..'))
		{
			html_init("gb2312");
                        html_error_quit("��ȡ�ʼ�����ʧ��!");
                        die();
                }
		
		if($act == "del")
		{
			$filename = $_GET["file"];
			$ret = bbs_delmail($dirname,$filename);
			if($ret != 0)
			{
				html_init("gb2312");
				html_error_quit("�ż������ڻ��������, �޷�ɾ��");
				die();
			}
		}
		elseif($act == "move")
		{
			for($i=0;$i < $mail_num;$i++)
			{
				if(!isset($_POST["file".$i])||$_POST["file".$i]=="")
					continue;
				
				$filename = $_POST["file".$i];
				if( $act2 == "delarea" )
					$ret = bbs_delmail($dirname,$filename);
			}
		}
		elseif($act == "clear")
		{
			
			$mail_fullpath = bbs_setmailfile($currentuser["userid"],".DELETED");
			$mail_num = bbs_getmailnum2($mail_fullpath);
			$maildata = bbs_getmails($mail_fullpath,0,$mail_num);
			for($i=0; $i < $mail_num; $i++ )
			{
				bbs_delmail(".DELETED",$maildata[$i]["FILENAME"]);
			}
			header("Location:bbsmail.php");
			die();
		}
		/*
		elseif($act == "delarea" )
		{
			$mail_fullpath = bbs_setmailfile($currentuser["userid"],$dirname);
			$mail_num = bbs_getmailnum2($mail_fullpath);
			
			$dstart = $_POST["dstart"];
			$dend = $_POST["dend"];
			$dtype = $_POST["dtype"];
			
			if( $dstart < 1 || $dstart > $mail_num  || $dend < 1 || $dend > $mail_num  || $dstart > $dend  )
			{
				html_init("gb2312");
				html_error_quit("����ɾ����ʼ����������������������룡");
				die();
			}
			
			$dnum = $dend - $dstart + 1;
			$dstart-- ;
			
			$maildata = bbs_getmails($mail_fullpath,$dstart,$dnum);
			if($dtype == 1)
			{
				foreach( $maildata as $mail )
				{
					bbs_delmail($dirname,$mail["FILENAME"]);
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
						bbs_delmail($dirname,$mail["FILENAME"]);
					}
				}
			}
		}
		*/
		else
		{
			
		}
		
		header("Location:bbsmailbox.php?path=".urlencode($dirname)."&title=".urlencode($title));
	}
?>