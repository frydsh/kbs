<?php
	/**
	 * send mail .
	 * $Id$
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
		if (! bbs_can_send_mail() )
			html_error_quit("�����ܷ����ż�");
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		if (isset($_GET["file"]))
			$file = $_GET["file"];
		if (isset($_GET["title"]))
			$title = $_GET["title"];
		if (isset($_GET["userid"]))
			$destuserid = $_GET["userid"];

		if (isset( $board )){
			$brdarr = array();
			$brdnum = bbs_getboard($board, $brdarr);
			if ($brdnum == 0)
				html_error_quit("�����������");
			$usernum = $currentuser["index"];
			if (bbs_checkreadperm($usernum, $brdnum) == 0)
				html_error_quit("�����������");
			if (isset($file) && bbs_valid_filename($file) < 0)
				html_error_quit("���������");
		}else{
			if (isset($file) && ( $file[0]!='M' || strstr($file,"..") ) )
				html_error_quit("���������..");
		}
		$brd_encode = urlencode($brdarr["NAME"]);
	}
?>
<body>
<center>
<?php echo BBS_FULL_NAME; ?> -- �����Ÿ� [ʹ����: <?php echo $currentuser["userid"]; ?>]
<hr class="default" />
<form name="postform" method="post" action="/cgi-bin/bbs/bbssndmail">
<table border="1">
<tr>
<td>
������: <?php echo $currentuser["userid"]; ?><br />
�ż�����: <input type="text" name="title" size="40" maxlength="100" value="<?php echo $title; ?>"><br />
������: <input type="text" name="userid" value="<?php echo $destuserid; ?>"><br />
ʹ��ǩ���� <select name="signature">
<?php
		if ($currentuser["signature"] == 0)
		{
?>
<option value="0" selected="selected">��ʹ��ǩ����</option>
<?php
		}
		else
		{
?>
<option value="0">��ʹ��ǩ����</option>
<?php
			for ($i = 1; $i < 6; $i++)
			{
				if ($currentuser["signature"] == $i)
				{
?>
<option value="<?php echo $i; ?>" selected="selected">�� <?php echo $i; ?> ��</option>
<?php
				}
				else
				{
?>
<option value="<?php echo $i; ?>">�� <?php echo $i; ?> ��</option>
<?php
				}
			}
		}
?>
</select>
 [<a target="_balnk" href="bbssig.php">�鿴ǩ����</a>]
<input type="checkbox" name="backup">����<br />
<textarea name="text"  onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.postform.submit(); return false;}'  onkeypress='if(event.keyCode==10) return document.postform.submit()' rows="20" cols="80" wrap="physical">
<?php
    if(isset($file)){
		if(isset($board)){
    		$filename = "boards/" . $board . "/" . $file;
            echo "\n�� �� " . $destuserid . " �Ĵ������ᵽ: ��\n";
		}else{
			$filename = "mail/".strtoupper($destuserid{0})."/".$destuserid."/".$file;
            echo "\n�� �� " . $destuserid . " ���������ᵽ: ��\n";
		}
		if(file_exists($filename))
		{
		    $fp = fopen($filename, "r");
	        if ($fp) {
			    $lines = 0;
	            $buf = fgets($fp,256);       /* ȡ����һ���� ���������µ� ������Ϣ */
				$end = strrpos($buf,")");
				$start = strpos($buf,":");
				if($start != FALSE && $end != FALSE)
				    $quser=substr($buf,$start+2,$end-$start-1);

		        for ($i = 0; $i < 3; $i++) {
	                if (($buf = fgets($fp,500)) == FALSE)
	   	                break;
	            }
	            while (1) {
	                if (($buf = fgets($fp,500)) == FALSE)
	                    break;
	                if (strncmp($buf, ": ��", 4) == 0)
	                    continue;
	                if (strncmp($buf, ": : ", 4) == 0)
	                    continue;
	                if (strncmp($buf, "--\n", 3) == 0)
	                    break;
	                if (strncmp($buf,'\n',1) == 0)
	                    continue;
	                if (++$lines > 10) {
	                    echo ": ...................\n";
	                    break;
	                }
	                /* */
	                if (stristr($buf, "</textarea>") == FALSE)  //filter </textarea> tag in the text
	                    echo ": ". $buf;
	            }
	            fclose($fp);
	        }
	    }
	}
?>
</textarea><br><div align="center">
<input type="submit" value="����" />
<input type="reset" value="���" />
<input type="button" value="����" onclick="window.location.href='bbsmail.php'" />
</form></div></table>
</html>
