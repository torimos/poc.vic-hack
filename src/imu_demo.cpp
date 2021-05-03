#include "utils.h"
#include "lcd.h"
#include "imu.h"


int imu_spi_read(int fd, unsigned char* buff, uint32_t tx_len, uint32_t rx_len)
{
    struct spi_ioc_transfer xfer[2];
    memset(xfer, 0, sizeof xfer);
	buff[0] |= 1 << BMI160_SPI_READ_BIT;
    xfer[0].tx_buf = (unsigned long)buff;
    xfer[0].len = tx_len;
    xfer[1].rx_buf = (unsigned long)buff;
    xfer[1].len = rx_len;
    return ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
}

int imu_spi_write(int fd, unsigned char* buff, uint32_t len)
{
	struct spi_ioc_transfer xfer[1];
    memset(xfer, 0, sizeof xfer);
    xfer[0].tx_buf = (unsigned long)buff;
    xfer[0].len = len;
    return ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
}

uint8_t reg_read(int fd, uint8_t reg)
{
	uint8_t data = reg;
	imu_spi_read(fd, &data, 1, 1);
	return data;
}

uint8_t reg_read_bits(int fd, uint8_t reg, unsigned pos, unsigned len)
{
    uint8_t b = reg_read(fd, reg);
    uint8_t mask = (1 << len) - 1;
    b >>= pos;
    b &= mask;
    return b;
}

void reg_write(int fd, uint8_t reg, uint8_t data)
{
	uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
	imu_spi_write(fd, buffer, 2);
}

void reg_write_bits(int fd, uint8_t reg, uint8_t data, unsigned pos, unsigned len)
{
    uint8_t b = reg_read(fd, reg);
    uint8_t mask = ((1 << len) - 1) << pos;
    data <<= pos; // shift data into correct position
    data &= mask; // zero all non-important bits in data
    b &= ~(mask); // zero all important bits in existing byte
    b |= data; // combine data with existing byte
    reg_write(fd, reg, b);
}

float convertRawData(int raw) {
  // -500 maps to a raw value of -32768
  // +500 maps to a raw value of 32767
  float v = (raw * 500.0) / 32768.0;
  return v;
}

void getGyro(int spi, float* gx, float* gy, float* gz)
{
	int gxRaw, gyRaw, gzRaw;
	uint8_t buffer[6];
	buffer[0] = BMI160_RA_GYRO_X_L;
	imu_spi_read(spi, buffer, 1, 6);
	gxRaw = (((int16_t)buffer[1]) << 8) | buffer[0];
	gyRaw = (((int16_t)buffer[3]) << 8) | buffer[2];
	gzRaw = (((int16_t)buffer[5]) << 8) | buffer[4];
	*gx = convertRawData(gxRaw);
	*gy = convertRawData(gyRaw);
	*gz = convertRawData(gzRaw);
}

void getAccel(int spi, float* ax, float* ay, float* az)
{
	int axRaw, ayRaw, azRaw;
	uint8_t buffer[6];
	buffer[0] = BMI160_RA_ACCEL_X_L;
	imu_spi_read(spi, buffer, 1, 6);
	axRaw = (((int16_t)buffer[1]) << 8) | buffer[0];
	ayRaw = (((int16_t)buffer[3]) << 8) | buffer[2];
	azRaw = (((int16_t)buffer[5]) << 8) | buffer[4];
	*ax = convertRawData(axRaw);
	*ay = convertRawData(ayRaw);
	*az = convertRawData(azRaw);
}

void imuInit(int spi)
{
	reg_write(spi, BMI160_RA_CMD, BMI160_CMD_SOFT_RESET);
    delay(1);
	reg_read(spi, 0x7F); // Issue a dummy-read to force the device into SPI comms mode
    delay(1);

	reg_write(spi, BMI160_RA_CMD, BMI160_CMD_ACC_MODE_NORMAL);
    delay(1);
    /* Wait for power-up to complete */
    while (0x1 != reg_read_bits(spi, BMI160_RA_PMU_STATUS,
                                BMI160_ACC_PMU_STATUS_BIT,
                                BMI160_ACC_PMU_STATUS_LEN))
	delay(1);

	reg_write(spi, BMI160_RA_CMD, BMI160_CMD_GYR_MODE_NORMAL);
    delay(1);
    /* Wait for power-up to complete */
    while (0x1 != reg_read_bits(spi, BMI160_RA_PMU_STATUS,
                                BMI160_GYR_PMU_STATUS_BIT,
                                BMI160_GYR_PMU_STATUS_LEN))
	delay(1);

	reg_write_bits(spi, BMI160_RA_GYRO_RANGE, BMI160_GYRO_RANGE_500,
                   BMI160_GYRO_RANGE_SEL_BIT,
                   BMI160_GYRO_RANGE_SEL_LEN);
	reg_write_bits(spi, BMI160_RA_ACCEL_RANGE, BMI160_ACCEL_RANGE_2G,
                   BMI160_ACCEL_RANGE_SEL_BIT,
                   BMI160_ACCEL_RANGE_SEL_LEN);

	// reg_write(spi, BMI160_RA_GYRO_CONF, 0x09);
	// reg_write(spi, BMI160_RA_ACCEL_CONF, 0x19);
	// reg_write(spi, BMI160_RA_INT_OUT_CTRL, 0x44);
	// reg_write(spi, BMI160_USER_FIFO_DOWN_ADDR, 0x88);
	// reg_write(spi, BMI160_RA_FIFO_CONFIG_0, 0xC8);
	// reg_write(spi, BMI160_RA_FIFO_CONFIG_1, 0xC2);
}

int imu_demo()
{

	int spi = open("/dev/spidev0.0", O_RDWR);
    if (spi < 0) {
        perror("open");
        return 1;
    }
	
	uint8_t mode = 0;
	uint8_t bits = 8;
	uint32_t speed = 15000000;
    ioctl(spi, SPI_IOC_WR_MODE, (char *)&mode);
    ioctl(spi, SPI_IOC_WR_BITS_PER_WORD, (char *)&bits);
    ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, (char *)&speed);
    printf("[%lld] IMU SPI Mode=%04x Bits=%02d Speed=%04d\n", get_time(), mode,bits,speed);

	imuInit(spi);
	uint8_t reg = reg_read(spi, 0);
    printf("BMI160 Initialized. ID = %x\n",reg);
	delay(10);

	float gx, gy, gz;
	float ax, ay, az;
	for (;;) {
		getGyro(spi, &gx,&gy,&gz);
		getAccel(spi, &ax,&ay,&az);
		printf("gX: %3.3f\tgY: %3.3f\tgZ: %3.3f\n",gx,gy,gz);
		printf("aX: %3.3f\taY: %3.3f\taZ: %3.3f\n",ax,ay,az);
		delay(100);
	}

	return 0;
}