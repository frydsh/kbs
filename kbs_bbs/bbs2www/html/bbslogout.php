<?php
	/**
	 * This file handles user logout.
	 * $Id$
	 */
  require("funcs.php");
  if ($loginok!=1) {
?>
<html>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="/bbs.css">
û�е�½<br><br>
<a href="/index.html">���ٷ���</a>
</html>
<?php
 } else {
	bbs_wwwlogoff();
    setcookie("UTMPKEY","",time()-3600,"/");
    setcookie("UTMPNUM","",time()-3600,"/");
    setcookie("UTMPUSERID","",time()-3600,"/");
    setcookie("LOGINTIME","",time()-3600,"/");

	header("Location: /index.html");
   }
?>
