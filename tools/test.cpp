

int main()
{
    int a, b, c, d;
    int buffer[20];

    a = 99343; b = 11235;
    c = a + b;
    d = a * 2;

    for(int i = 0; i < 20; i++)
    {
        buffer[i] = i + d;
    }

    return 0;
}
