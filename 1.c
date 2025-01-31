using System;
using System.Net.Http;
using System.Threading.Tasks;
using Microsoft.Extensions.Caching.Memory;

public class IMDSv2TokenService
{
    private readonly HttpClient _httpClient;
    private readonly IMemoryCache _cache;
    private const string IMDS_BASE_URL = "http://169.254.169.254/latest/api/token";
    private const int TOKEN_TTL_SECONDS = 21600; // Máximo de 6 horas

    public IMDSv2TokenService(HttpClient httpClient, IMemoryCache cache)
    {
        _httpClient = httpClient;
        _cache = cache;
    }

    public async Task<string> GetTokenAsync()
    {
        if (!_cache.TryGetValue("IMDSv2Token", out string token))
        {
            token = await GenerateTokenAsync();
            var cacheEntryOptions = new MemoryCacheEntryOptions()
                .SetAbsoluteExpiration(TimeSpan.FromSeconds(TOKEN_TTL_SECONDS - 300)); // Renova 5 min antes de expirar

            _cache.Set("IMDSv2Token", token, cacheEntryOptions);
        }

        return token;
    }

    private async Task<string> GenerateTokenAsync()
    {
        var request = new HttpRequestMessage(HttpMethod.Put, IMDS_BASE_URL);
        request.Headers.Add("X-aws-ec2-metadata-token-ttl-seconds", TOKEN_TTL_SECONDS.ToString());

        var response = await _httpClient.SendAsync(request);
        response.EnsureSuccessStatusCode();

        return await response.Content.ReadAsStringAsync();
    }
}
