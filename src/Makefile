video:
	rm gource.txt
	gource --output-custom-log gource.txt
	(IFS="|"; while read wrong_name name; do sed -i "s/|$$wrong_name|/|$$name|/g" gource.txt; done < alias.txt)
	gource -f --path gource.txt --start-date '2024-05-24 07:00:00' --user-image-dir avatars -c 3
