常用接口：

1、加载文件    
// load from a data file
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile("example.ini");

2、获取值

ini.GetLongValue("Cfg0", "Period ",  5);

ini.GetValue("section1", "key99");

3、修改数据

ini.SetValue("section1", nullptr, nullptr);

rc = cfgini.SetLongValue("Cfg0", "Period ", 6, "Period");

4、删除数据

ini.Delete("Cfg0", "Period ");

done = ini.Delete("section2", nullptr);


5、保存数据
    // save the data to a string
    std::string data;
    rc = ini.Save(data);
    rc = ini.SaveFile(FILE_NAME);
    if (rc < 0) { 
        printf("保存 %s ini文件失败\n", FILE_NAME);
    }
————————————————
版权声明：本文为CSDN博主「香飘飘cj」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/c4679281314/article/details/122951274

https://blog.csdn.net/cpp_learner/article/details/128780799