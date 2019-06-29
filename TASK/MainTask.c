
static bool flag = false;

void DS3231Task(void const *argument)
{
    for (;;)
    {
        while (!flag)
            ;
        struct tm t;
        uint8_t h;
        uint8_t m;
        uint8_t s;
        char buf[] = "00:00:00";
        static bool flag = false;
        if (DS3231_GetTime(&t))
        {
            h = t.tm_hour;
            m = t.tm_min;
            s = t.tm_sec;
            flag ? sprintf(buf, "%02d:%02d:%02d", h, m, s) : sprintf(buf, "%02d %02d %02d", h, m, s);
            flag = !flag;
            NixieTube_Show(buf);
        }
    }
}