#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE        (256)
#define DICE_COUNT      (2)

typedef struct {
    unsigned int bufSize;
    unsigned int index;
    unsigned char* buf;
} rndctx_t;

void resetBuffer(rndctx_t *ctx)
{
    ctx->index = 0;
    FILE *f = fopen("/dev/urandom", "r");
    for (unsigned int i=0; i<ctx->bufSize; i++)
        ctx->buf[i] = fgetc(f);
    fclose(f);
}

rndctx_t getNewContext(unsigned int bufSize)
{
    rndctx_t ctx;
    ctx.bufSize = bufSize;
    ctx.index = 0;
    ctx.buf = malloc(sizeof(unsigned char)*bufSize);
    resetBuffer(&ctx);
    return ctx;
}

void destroyContext(rndctx_t *ctx)
{
    free(ctx->buf);
}

unsigned char getRandomChar(rndctx_t *ctx, unsigned char max)
{
    unsigned char ret = ctx->buf[ctx->index];
    unsigned char maxSource = (256/max)*max;
    ctx->index++;
    if (ctx->index >= ctx->bufSize)
        resetBuffer(ctx);
    if (ret < maxSource)
        return ret%max;
    else
        return getRandomChar(ctx, max);
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

void test()
{
    rndctx_t ctx = getNewContext(BUF_SIZE);
    int counters[12];

    memset(counters, 0x00, sizeof(counters));
    printf("Testing distribution for 100 values\n");
    for (int i=0; i<100; i++) {
        char n = getRandomChar(&ctx, 6);
        counters[n]++;
    }
    for (int i=0; i<6; i++) {
        char bar[41];
        bar[40] = 0x00;
        memset(bar, '-', 40);
        memset(bar, 'X', min(40, counters[i]));
        printf("%d %4d %s\n", i+1, counters[i], bar);
    }

    memset(counters, 0x00, sizeof(counters));
    printf("Testing distribution for 100000 values\n");
    for (int i=0; i<100000; i++) {
        char n = getRandomChar(&ctx, 6);
        counters[n]++;
    }
    for (int i=0; i<6; i++) {
        char bar[41];
        bar[40] = 0x00;
        memset(bar, '-', 40);
        memset(bar, 'X', min(40, counters[i]/1000));
        printf("%d %6d %s\n", i+1, counters[i], bar);
    }

    memset(counters, 0x00, sizeof(counters));
    printf("Testing distribution for 100 values, two dice\n");
    for (int i=0; i<100; i++) {
        char n1 = getRandomChar(&ctx, 6);
        char n2 = getRandomChar(&ctx, 6);
        counters[n1+n2]++;
    }
    for (int i=0; i<11; i++) {
        char bar[41];
        bar[40] = 0x00;
        memset(bar, '-', 40);
        memset(bar, 'X', min(40, counters[i]));
        printf("%2d %6d %s\n", i+2, counters[i], bar);
    }

    memset(counters, 0x00, sizeof(counters));
    printf("Testing distribution for 100000 values, two dice\n");
    for (int i=0; i<100000; i++) {
        char n1 = getRandomChar(&ctx, 6);
        char n2 = getRandomChar(&ctx, 6);
        counters[n1+n2]++;
    }
    for (int i=0; i<11; i++) {
        char bar[41];
        bar[40] = 0x00;
        memset(bar, '-', 40);
        memset(bar, 'X', min(40, counters[i]/1000));
        printf("%2d %6d %s\n", i+2, counters[i], bar);
    }
}

rndctx_t theCtx;

typedef struct {
    bool spot[3][3];
} dice_t;

dice_t getDice(char number)
{
    bool dice[3][3];
    memset(dice, 0x00, sizeof(dice));
    switch (number) {
        case 1:
            dice[1][1] = true;
            break;
        case 2:
            dice[0][0] = true;
            dice[2][2] = true;
            break;
        case 3:
            dice[0][0] = true;
            dice[1][1] = true;
            dice[2][2] = true;
            break;
        case 4:
            dice[0][0] = true;
            dice[0][2] = true;
            dice[2][0] = true;
            dice[2][2] = true;
            break;
        case 5:
            dice[0][0] = true;
            dice[0][2] = true;
            dice[1][1] = true;
            dice[2][0] = true;
            dice[2][2] = true;
            break;
        case 6:
            dice[0][0] = true;
            dice[0][1] = true;
            dice[0][2] = true;
            dice[2][0] = true;
            dice[2][1] = true;
            dice[2][2] = true;
            break;
        default:
            printf("Bad number for dice (%d)\n", number);
    }
    dice_t ret;
    memcpy(&ret.spot, dice, sizeof(dice));
    return ret;
}

void drawDiceLine(const dice_t *dice, int index)
{
    if (index == 0 || index == 4) {
        printf("+-------+");
        return;
    }
    printf("| %s %s %s |",
        dice->spot[0][index-1] ? "o" : " ",
        dice->spot[1][index-1] ? "o" : " ",
        dice->spot[2][index-1] ? "o" : " ");
}

void drawAllDice()
{
    dice_t d[6];
    for (int i=0; i<6; i++)
        d[i] = getDice(i+1);
    for (int i=0; i<5; i++) {
        for (int j=0; j<6; j++) {
            drawDiceLine(&d[j], i);
            printf("   ");
        }
        printf("\n");
    }
}

void printHelpText()
{
    printf("Available commands:\n");
    printf("  test - run some test rolls to check uniform distribution\n");
    printf("  dice - print all available dice faces\n");
    printf("  exit - exit application\n");
    printf("  help - print this help text\n");
    printf("To roll the dice, type anything and hit <enter>\n");
}

void parseCommand(const char *cmd)
{
    if (strcmp(cmd, "test") == 0) {
        test();
        return;
    }
    if (strcmp(cmd, "dice") == 0) {
        drawAllDice();
        return;
    }
    if (strcmp(cmd, "help") == 0) {
        printHelpText();
        return;
    }
    if (strcmp(cmd, "exit") == 0) {
        printf("Application will exit now.\n");
        return;
    }
    unsigned char d1, d2;
    d1 = getRandomChar(&theCtx, 6)+1;
    d2 = getRandomChar(&theCtx, 6)+1;
    printf("New roll %d, %d (%d)\n", d1, d2, d1+d2);
    dice_t dice1 = getDice(d1);
    dice_t dice2 = getDice(d2);
    for (int i=0; i<5; i++) {
        drawDiceLine(&dice1, i);
        printf("   ");
        drawDiceLine(&dice2, i);
        printf("\n");
    }
}

int main()
{
    theCtx = getNewContext(BUF_SIZE);
    char cmd[256];
    printf("Type command\n");
    while (strcmp(cmd, "exit")) {
        printf("> ");
        scanf("%s", cmd);
        parseCommand(cmd);
    }
    destroyContext(&theCtx);
}
