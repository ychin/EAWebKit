BAD_CONSTANT = '-2147483648'
GOOD_CONSTANT = 'INT_MIN'

if ARGV.length < 1
    print 'You must provide a source file'
    exit 1
end

source_file = ARGV[0]

f = File.open(source_file, "r")
f.each_line do |line|
    if line.include? BAD_CONSTANT
        puts line.gsub(BAD_CONSTANT, GOOD_CONSTANT)
    else
        puts line
    end
end
f.close
